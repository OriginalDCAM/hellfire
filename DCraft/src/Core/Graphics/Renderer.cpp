//
// Simplified Renderer.cpp
//
#include "DCraft/Graphics/Renderer.h"
#include "GL/glew.h"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>


#include "DCraft/Components/CameraComponent.h"
#include "DCraft/Components/LightComponent.h"
#include "DCraft/Graphics/Renderers/SkyboxRenderer.h"
#include "DCraft/Structs/Scene.h"

namespace DCraft {
    Renderer::Renderer(uint32_t fallback_program_id) : fallback_program_id_(fallback_program_id),
                                                       render_to_framebuffer_(false), framebuffer_width_(800),
                                                       framebuffer_height_(600) {
        OGLRendererContext *ogl_context = new OGLRendererContext();
        ogl_context->default_shader_program = fallback_program_id;
        context_ = static_cast<void *>(ogl_context);
    }

    Renderer::Renderer()
        : fallback_program_id_(0), render_to_framebuffer_(false),
          framebuffer_width_(800), framebuffer_height_(600) {
        OGLRendererContext *ogl_context = new OGLRendererContext();
        ogl_context->default_shader_program = 0;
        context_ = static_cast<void *>(ogl_context);
    }

    Renderer::~Renderer() {
        if (context_) {
            OGLRendererContext *ogl_context = static_cast<OGLRendererContext *>(context_);
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

        skybox_renderer_.initialize();
    }

    void Renderer::render(Scene &scene) {
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

        opaque_objects_.clear();
        transparent_objects_.clear();
    }

    void Renderer::end_frame() {
        glFlush();
    }

    uint32_t Renderer::get_default_shader() const {
        return fallback_program_id_;
    }

    void *Renderer::get_context() const {
        return context_;
    }

    void Renderer::store_lights_in_context(const std::vector<Entity *> &light_entities, CameraComponent &camera) {
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
                    // Handle spot lights if needed
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

        // 1. Collect light data and store in render context
        std::vector<Entity *> light_entities = scene.find_entities_with_component<LightComponent>();
        store_lights_in_context(light_entities, camera);

        // 2. Collect render commands
        auto *camera_transform = camera.get_owner()->get_component<TransformComponent>();
        glm::vec3 camera_pos = camera_transform ? camera_transform->get_world_position() : glm::vec3(0.0f);

        for (Entity *entity: scene.get_entities()) {
            collect_render_commands_recursive(entity, camera_pos);
        }

        // 3. Render in proper order
        glm::mat4 view = camera.get_view_matrix();
        glm::mat4 projection = camera.get_projection_matrix();

        render_opaque_pass(view, projection);
        render_skybox_pass(&scene, view, projection);
        render_transparent_pass(view, projection);
    }

    void Renderer::collect_render_commands_recursive(Entity *entity, const glm::vec3 &camera_pos) {
        // Check if this entity is renderable
        if (auto *renderable = entity->get_component<RenderableComponent>()) {
            if (auto *transform = entity->get_component<TransformComponent>()) {
                if (renderable->has_mesh()) {
                    Material *material = renderable->get_material();
                    if (material) {
                        glm::vec3 object_pos = transform->get_world_position();
                        float distance = glm::length(camera_pos - object_pos);
                        bool is_transparent = is_material_transparent(material);

                        RenderCommand cmd = {entity, renderable, material, distance, is_transparent};

                        if (is_transparent) {
                            transparent_objects_.push_back(cmd);
                        } else {
                            opaque_objects_.push_back(cmd);
                        }
                    }
                }
            }
        }

        for (Entity *child: entity->get_children()) {
            collect_render_commands_recursive(child, camera_pos);
        }
    }


    bool Renderer::is_material_transparent(Material *material) {
        float transparency = material->get_property<float>("transparency", 1.0f);
        float alpha = material->get_property<float>("alpha", 1.0f);
        bool use_transparency = material->get_property<bool>("useTransparency", false);
        return transparency < 1.0f || alpha < 1.0f || use_transparency;
    }

    void Renderer::render_opaque_pass(const glm::mat4 &view, const glm::mat4 &projection) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        std::sort(opaque_objects_.begin(), opaque_objects_.end(),
                  [](const RenderCommand &a, const RenderCommand &b) {
                      return a.distance_to_camera < b.distance_to_camera;
                  });

        for (const auto &cmd: opaque_objects_) {
            cmd.renderable->draw(view, projection, fallback_program_id_, context_);
        }
    }

    void Renderer::render_transparent_pass(const glm::mat4 &view, const glm::mat4 &projection) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        std::sort(transparent_objects_.begin(), transparent_objects_.end(),
                  [](const RenderCommand &a, const RenderCommand &b) {
                      return a.distance_to_camera > b.distance_to_camera;
                  });

        for (const auto &cmd: transparent_objects_) {
            cmd.renderable->draw(view, projection, fallback_program_id_, context_);
        }

        glDepthMask(GL_TRUE);
    }

    void Renderer::render_skybox_pass(Scene *scene, const glm::mat4 &view, const glm::mat4 &projection) const {
        if (!scene || !scene->has_skybox()) return;

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

    void Renderer::set_fallback_shader(uint32_t shader_program_id) {
        fallback_program_id_ = shader_program_id;

        if (context_) {
            OGLRendererContext *ogl_context = static_cast<OGLRendererContext *>(context_);
            ogl_context->default_shader_program = shader_program_id;
        }
    }
}
