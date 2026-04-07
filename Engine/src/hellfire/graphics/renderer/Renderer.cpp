//
// Simplified Renderer.cpp
//
#include "Renderer.h"
#include "GL/glew.h"
#include <algorithm>


#include "hellfire/core/Application.h"
#include "hellfire/ecs/CameraComponent.h"
#include "hellfire/ecs/InstancedRenderableComponent.h"
#include "hellfire/ecs/LightComponent.h"
#include "hellfire/ecs/components/MeshComponent.h"
#include "hellfire/graphics/renderer/SkyboxRenderer.h"
#include "hellfire/scene/Scene.h"

namespace hellfire {
    Renderer::Renderer(uint32_t fallback_program_id) : shader_registry_(&shader_manager_), fallback_shader_(nullptr),
                                                       fallback_program_(fallback_program_id),
                                                       render_to_framebuffer_(false), framebuffer_width_(800),
                                                       framebuffer_height_(600) {
        auto *ogl_context = new OGLRendererContext();
        ogl_context->shader_handle = fallback_program_id;
        context_ = static_cast<void *>(ogl_context);
    }

    Renderer::Renderer()
        : shader_registry_(nullptr), fallback_shader_(nullptr), fallback_program_(0), render_to_framebuffer_(false),
          framebuffer_width_(800), framebuffer_height_(600) {
        auto *ogl_context = new OGLRendererContext();
        ogl_context->shader_handle = 0;
        context_ = static_cast<void *>(ogl_context);
    }

    Renderer::~Renderer() {
        if (context_) {
            const auto *ogl_context = static_cast<OGLRendererContext *>(context_);
            delete ogl_context;
            context_ = nullptr;
        }
    }

    void Renderer::init() {
        // TODO: Read renderer options from a config file
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDepthFunc(GL_LESS);

        glDisable(GL_CULL_FACE);

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Makes errors appear immediately
        glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, 
                                  GLenum severity, GLsizei length, 
                                  const GLchar* message, const void* userParam) {
            if (type == GL_DEBUG_TYPE_ERROR) {
                std::cerr << "GL ERROR: " << message << std::endl;
                __debugbreak();
                // Optionally set a breakpoint here
            }
        }, nullptr);

        skybox_renderer_.initialize();
    }

    void Renderer::render(Scene &scene, float time) {
        CameraComponent *camera = scene.get_active_camera();
        if (!camera) {
            std::cerr << "No active camera in scene!" << std::endl;
            return;
        }

        if (render_to_framebuffer_) {
            render_scene_to_framebuffer(scene, *camera);
        } else {
            begin_frame();
            render_internal(scene, *camera);
            end_frame();
        }
    }

    void Renderer::begin_frame() {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        opaque_objects_.clear();
        transparent_objects_.clear();
        opaque_instanced_objects_.clear();
        transparent_instanced_objects_.clear();
    }

    void Renderer::end_frame() {
        glFlush();
    }

    Shader *Renderer::get_default_shader() const {
        return fallback_shader_;
    }

    void *Renderer::get_context() const {
        return context_;
    }

    void Renderer::store_lights_in_context(const std::vector<Entity *> &light_entities, CameraComponent &camera) const {
        OGLRendererContext *ogl_context = static_cast<OGLRendererContext *>(context_);
        if (!ogl_context) return;

        // Separate lights by type
        std::vector<Entity *> directional_lights;
        std::vector<Entity *> point_lights;

        for (Entity *entity: light_entities) {
            auto *light = entity->get_component<LightComponent>();
            if (!light) continue;

            switch (light->get_light_type()) {
                case LightComponent::LightType::DIRECTIONAL:
                    if (directional_lights.size() < 4) {
                        directional_lights.push_back(entity);
                    }
                    break;
                case LightComponent::LightType::POINT:
                    if (point_lights.size() < 8) {
                        point_lights.push_back(entity);
                    }
                    break;
                case LightComponent::LightType::SPOT:
                    // TODO: Handle spot lights
                    break;
            }
        }

        // Store light counts
        ogl_context->num_directional_lights = static_cast<int>(directional_lights.size());
        ogl_context->num_point_lights = static_cast<int>(point_lights.size());

        // Store light entities (you may need to adapt your context structure)
        for (int i = 0; i < ogl_context->num_directional_lights; i++) {
            ogl_context->directional_light_entities[i] = directional_lights[i];
        }

        for (int i = 0; i < ogl_context->num_point_lights; i++) {
            ogl_context->point_light_entities[i] = point_lights[i];
        }

        // Store camera component
        ogl_context->camera_component = &camera;
    }

    void Renderer::render_internal(Scene &scene, CameraComponent &camera) {
        opaque_objects_.clear();
        transparent_objects_.clear();
        opaque_instanced_objects_.clear();
        transparent_instanced_objects_.clear();

        // Store this for collect methods
        scene_ = &scene;

        // Collect lights
        std::vector<EntityID> light_entity_ids = scene.find_entities_with_component<LightComponent>();
        std::vector<Entity *> light_entities;
        for (const EntityID id: light_entity_ids) {
            if (Entity *e = scene.get_entity(id)) {
                light_entities.push_back(e);
            }
        }
        store_lights_in_context(light_entities, camera);

        // Get camera position
        EntityID camera_entity_id = scene.get_active_camera_entity();
        Entity *camera_entity = scene.get_entity(camera_entity_id);
        auto *camera_transform = camera_entity ? camera_entity->get_component<TransformComponent>() : nullptr;
        glm::vec3 camera_pos = camera_transform ? camera_transform->get_world_position() : glm::vec3(0.0f);

        // Collect render commands from root entities
        for (EntityID root_id: scene.get_root_entities()) {
            collect_render_commands_recursive(root_id, camera_pos);
        }

        // Render
        glm::mat4 view = camera.get_view_matrix();
        glm::mat4 projection = camera.get_projection_matrix();

        render_opaque_pass(view, projection);
        render_skybox_pass(&scene, view, projection);
        render_transparent_pass(view, projection);
    }

    void Renderer::collect_render_commands_recursive(EntityID entity_id, const glm::vec3 &camera_pos) {
        Entity *entity = scene_->get_entity(entity_id);
        if (!entity) return;

        // Check for renderable + mesh components
        auto *renderable = entity->get_component<RenderableComponent>();
        auto *mesh_comp = entity->get_component<MeshComponent>();
        auto *transform = entity->get_component<TransformComponent>();

        // Need all three to render
        if (renderable && mesh_comp && transform) {
            auto mesh = mesh_comp->get_mesh();
            auto material = renderable->get_material();

            if (mesh && material) {
                const glm::vec3 object_pos = transform->get_world_position();
                const float distance = glm::length(camera_pos - object_pos);
                const bool is_transparent = is_material_transparent(material);

                RenderCommand cmd = {entity_id, mesh, material, distance, is_transparent};

                if (is_transparent) {
                    transparent_objects_.push_back(cmd);
                } else {
                    opaque_objects_.push_back(cmd);
                }
            }
        }

        if (auto *instanced = entity->get_component<InstancedRenderableComponent>()) {
            if (transform && instanced->has_mesh() && instanced->get_instance_count() > 0) {
                if (auto material = instanced->get_material()) {
                    glm::vec3 object_pos = transform->get_world_position();
                    float distance = glm::length(camera_pos - object_pos);
                    bool is_transparent = is_material_transparent(material);

                    InstancedRenderCommand cmd = {entity_id, instanced, material, distance, is_transparent};

                    if (is_transparent) {
                        transparent_instanced_objects_.push_back(cmd);
                    } else {
                        opaque_instanced_objects_.push_back(cmd);
                    }
                }
            }
        }

        // Recurse through children using scene hierarchy
        for (EntityID child_id: scene_->get_children(entity_id)) {
            collect_render_commands_recursive(child_id, camera_pos);
        }
    }


    bool Renderer::is_material_transparent(const std::shared_ptr<Material> &material) {
        if (!material) {
            return false;
        }
        const auto transparency = material->get_property<float>("uTransparency", 1.0f);
        const auto alpha = material->get_property<float>("uAlpha", 1.0f);
        const bool use_transparency = material->get_property<bool>("useTransparency", false);
        return transparency < 1.0f || alpha < 1.0f || use_transparency;
    }

    void Renderer::render_opaque_pass(const glm::mat4 &view, const glm::mat4 &projection) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND);

        glDisable(GL_CULL_FACE);

        std::sort(opaque_objects_.begin(), opaque_objects_.end(),
                  [](const RenderCommand &a, const RenderCommand &b) {
                      return a.distance_to_camera < b.distance_to_camera;
                  });

        for (const auto &cmd: opaque_objects_) {
            draw_render_command(cmd, view, projection);
        }

        for (const auto &cmd: opaque_instanced_objects_) {
            draw_instanced_command(cmd, view, projection);
        }
    }

    void Renderer::render_transparent_pass(const glm::mat4 &view, const glm::mat4 &projection) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        glDisable(GL_CULL_FACE);

        std::sort(transparent_objects_.begin(), transparent_objects_.end(),
                  [](const RenderCommand &a, const RenderCommand &b) {
                      return a.distance_to_camera > b.distance_to_camera;
                  });

        for (const auto &cmd: transparent_objects_) {
            draw_render_command(cmd, view, projection);
        }

        for (const auto &cmd: transparent_instanced_objects_) {
            draw_instanced_command(cmd, view, projection);
        }

        glDepthMask(GL_TRUE);
    }

    void Renderer::draw_render_command(const RenderCommand &cmd, const glm::mat4 &view, const glm::mat4 &projection) {
        Entity *entity = scene_->get_entity(cmd.entity_id);
        if (!entity) return;

        auto *transform = entity->get_component<TransformComponent>();
        if (!transform) return;

        Shader *shader = get_shader_for_material(cmd.material);
        if (!shader) shader = fallback_shader_;

        glUseProgram(shader->get_program_id());

        // Upload lights
        if (context_) {
            RenderingUtils::upload_lights_to_shader(*shader, context_);
        }

        // Upload default uniforms
        RenderingUtils::set_standard_uniforms(*shader, transform->get_world_matrix(), view, projection);

        // Bind material and draw mesh
        cmd.material->bind();
        cmd.mesh->draw();
        cmd.material->unbind();
    }

    void Renderer::draw_instanced_command(const InstancedRenderCommand &cmd, const glm::mat4 &view,
                                          const glm::mat4 &projection) {
        Entity *entity = scene_->get_entity(cmd.entity_id);
        if (!entity) return;

        Shader *shader = get_shader_for_material(cmd.material);
        if (!shader) shader = fallback_shader_;

        glUseProgram(shader->get_program_id());

        // Upload lights
        if (context_) {
            RenderingUtils::upload_lights_to_shader(*shader, context_);
        }

        // Upload uniforms
        RenderingUtils::set_standard_uniforms(*shader, glm::mat4(1.0f), view, projection);

        // Prepare instanced data
        cmd.instanced_renderable->prepare_for_draw();

        // Bind material and draw
        cmd.material->bind();

        auto mesh = cmd.instanced_renderable->get_mesh();
        if (mesh) {
            mesh->bind();
            cmd.instanced_renderable->bind_instance_buffers();
            mesh->draw_instanced(cmd.instanced_renderable->get_instance_count());
            mesh->unbind();
        }

        cmd.material->unbind();
    }


    void Renderer::render_skybox_pass(Scene *scene, const glm::mat4 &view, const glm::mat4 &projection) const {
        if (!scene || !scene->has_skybox()) return;

        glDisable(GL_CULL_FACE);

        CameraComponent *camera_comp = scene->get_active_camera();
        if (camera_comp) {
            skybox_renderer_.render(scene->get_skybox(), camera_comp);
        }
    }

    void Renderer::create_scene_framebuffer(uint32_t width, uint32_t height) {
        framebuffer_width_ = width;
        framebuffer_height_ = height;
        scene_framebuffer_ = std::make_unique<Framebuffer>(width, height);
    }

    void Renderer::resize_scene_framebuffer(uint32_t width, uint32_t height) {
        if (scene_framebuffer_) {
            framebuffer_width_ = width;
            framebuffer_height_ = height;
            scene_framebuffer_->resize(width, height);
        } else {
            create_scene_framebuffer(width, height);
        }
    }

    uint32_t Renderer::get_scene_texture() const {
        if (scene_framebuffer_) {
            return scene_framebuffer_->get_color_texture();
        }
        return 0;
    }

    void Renderer::render_to_texture(Scene &scene, CameraComponent &camera, uint32_t width, uint32_t height) {
        if (!scene_framebuffer_ || scene_framebuffer_->get_width() != width || scene_framebuffer_->get_height() !=
            height) {
            resize_scene_framebuffer(width, height);
        }

        render_scene_to_framebuffer(scene, camera);
    }

    void Renderer::render_scene_to_framebuffer(Scene &scene, CameraComponent &camera) {
        if (!scene_framebuffer_) {
            create_scene_framebuffer(framebuffer_width_, framebuffer_height_);
        }

        scene_framebuffer_->bind();

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);

        render_internal(scene, camera);

        scene_framebuffer_->unbind();

        glFlush();
    }

    void Renderer::set_fallback_shader(Shader &fallback_shader) {
        fallback_shader_ = &fallback_shader;

        if (context_) {
            auto *ogl_context = static_cast<OGLRendererContext *>(context_);
            ogl_context->shader_handle = fallback_shader.get_program_id();
        }
    }

    Shader *Renderer::get_shader_for_material(std::shared_ptr<Material> material) {
        if (!material) {
            return fallback_shader_;
        }

        // Check if material has a compiled shader ID
        uint32_t material_shader_id = material->get_compiled_shader_id();
        if (material_shader_id != 0) {
            // Get shader wrapper from the ID
            Shader *material_shader = shader_registry_.get_shader_from_id(material_shader_id);
            if (material_shader) {
                return material_shader;
            }
        }

        // Check if material needs compilation
        if (material->has_custom_shader()) {
            // Try to compile the material's shader
            uint32_t compiled_id = compile_material_shader(material);
            if (compiled_id != 0) {
                material->set_compiled_shader_id(compiled_id);
                return shader_registry_.get_shader_from_id(compiled_id);
            }
        }

        // Fall back to default shader
        return fallback_shader_;
    }

    uint32_t Renderer::compile_material_shader(std::shared_ptr<Material> material) {
        if (!material || !material->has_custom_shader()) {
            return 0;
        }

        const Material::ShaderInfo *shader_info = material->get_shader_info();
        if (!shader_info) {
            return 0;
        }

        // Create shader variant with material's settings
        ShaderManager::ShaderVariant variant;
        variant.vertex_path = shader_info->vertex_path;
        variant.fragment_path = shader_info->fragment_path;
        variant.defines = shader_info->defines;

        // Add automatic defines based on material properties
        shader_manager_.add_automatic_defines(*material, variant.defines);

        // Compile using shader manager
        return shader_manager_.load_shader(variant);
    }
}
