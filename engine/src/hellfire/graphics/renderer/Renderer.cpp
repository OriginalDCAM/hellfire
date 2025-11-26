//
// Simplified Renderer.cpp
//
#include "Renderer.h"
#include "GL/glew.h"
#include <algorithm>


#include "hellfire/core/Application.h"
#include "hellfire/core/Time.h"
#include "hellfire/ecs/CameraComponent.h"
#include "hellfire/ecs/InstancedRenderableComponent.h"
#include "hellfire/ecs/LightComponent.h"
#include "hellfire/ecs/components/MeshComponent.h"
#include "hellfire/graphics/renderer/SkyboxRenderer.h"
#include "hellfire/scene/Scene.h"

namespace hellfire {
    Renderer::Renderer()
        : shader_registry_(nullptr), fallback_shader_(nullptr), fallback_program_(0), render_to_framebuffer_(false),
          framebuffer_width_(800), framebuffer_height_(600) {
        context_ = std::make_unique<OGLRendererContext>();
        context_->shader_handle = 0;
    }

    void Renderer::init() {
        // Enable debugging for OpenGL
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Makes errors appear immediately
        glDebugMessageCallback([](GLenum source, GLenum type, GLuint id,
                                  GLenum severity, GLsizei length,
                                  const GLchar *message, const void *userParam) {
            if (type == GL_DEBUG_TYPE_ERROR) {
                std::cerr << "GL ERROR: " << message << std::endl;
                __debugbreak();
            }
        }, nullptr);


        // Setup shadow pass shader
        shadow_material_ = MaterialBuilder::create_custom("Shadow Material", "assets/shaders/shadow.vert",
                                                          "assets/shaders/shadow.frag");

        skybox_renderer_.initialize();
    }

    void Renderer::render(Scene &scene, const Entity *camera_override = nullptr) {
        const Entity *camera_entity = camera_override;
        if (!camera_entity) {
            const EntityID camera_id = scene.get_default_camera_entity_id();
            camera_entity = scene.get_entity(camera_id);
        }

        if (!camera_entity) {
            std::cerr << "No active camera in scene!" << std::endl;
            return;
        }

        const auto camera_comp = camera_entity->get_component<CameraComponent>();

        if (!camera_comp) {
            std::cerr << "Camera entity missing CameraComponent" << std::endl;
        }

        render_frame(scene, *camera_comp);
    }

    void Renderer::reset_framebuffer_data() {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void Renderer::clear_draw_list() {
        opaque_objects_.clear();
        transparent_objects_.clear();
        opaque_instanced_objects_.clear();
        transparent_instanced_objects_.clear();
    }

    void Renderer::begin_frame() {
        reset_framebuffer_data();

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        clear_draw_list();
    }

    void Renderer::end_frame() {
        glFlush();
    }

    void Renderer::store_lights_in_context(const std::vector<Entity *> &light_entities, CameraComponent &camera) {
        if (!context_) return;

        // Separate lights by type
        std::vector<Entity *> directional_lights;
        std::vector<Entity *> point_lights;

        for (Entity *entity: light_entities) {
            const auto *light = entity->get_component<LightComponent>();
            if (!light) continue;

            // Sort lights into their respective vectors
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
        context_->num_directional_lights = static_cast<int>(directional_lights.size());
        context_->num_point_lights = static_cast<int>(point_lights.size());

        // Store light entities 
        for (int i = 0; i < context_->num_directional_lights; i++) {
            context_->directional_light_entities[i] = directional_lights[i];
        }

        for (int i = 0; i < context_->num_point_lights; i++) {
            context_->point_light_entities[i] = point_lights[i];
        }

        // Store camera component
        context_->camera_component = &camera;
    }

    void Renderer::collect_geometry_from_scene(Scene &scene, const glm::vec3 camera_pos) {
        for (const EntityID root_id: scene.get_root_entities()) {
            collect_render_commands_recursive(root_id, camera_pos);
        }
    }

    void Renderer::collect_render_commands_recursive(EntityID entity_id, const glm::vec3 &camera_pos) {
        const Entity *entity = scene_->get_entity(entity_id);
        if (!entity) return;

        // Check for renderable + mesh components
        const auto *renderable = entity->get_component<RenderableComponent>();
        const auto *mesh_comp = entity->get_component<MeshComponent>();
        const auto *transform = entity->get_component<TransformComponent>();

        // Need all three to render
        if (renderable && mesh_comp && transform) {
            const auto mesh = mesh_comp->get_mesh();
            const auto material = renderable->get_material();

            if (mesh && material) {
                const glm::vec3 object_pos = transform->get_world_position();
                const float distance = glm::length(camera_pos - object_pos);
                const bool is_transparent = material->is_transparent();

                const RenderCommand cmd = {entity_id, mesh, material, distance, is_transparent};

                if (is_transparent) {
                    transparent_objects_.push_back(cmd);
                } else {
                    opaque_objects_.push_back(cmd);
                }
            }
        }

        // If the entity has an Instancing component setup the render commands
        if (auto *instanced = entity->get_component<InstancedRenderableComponent>()) {
            if (transform && instanced->has_mesh() && instanced->get_instance_count() > 0) {
                if (const auto material = instanced->get_material()) {
                    const glm::vec3 object_pos = transform->get_world_position();
                    const float distance = glm::length(camera_pos - object_pos);
                    const bool is_transparent = material->is_transparent();

                    const InstancedRenderCommand cmd = {entity_id, instanced, material, distance, is_transparent};

                    if (is_transparent) {
                        transparent_instanced_objects_.push_back(cmd);
                    } else {
                        opaque_instanced_objects_.push_back(cmd);
                    }
                }
            }
        }

        // Recurse through children using scene hierarchy
        for (const EntityID child_id: scene_->get_children(entity_id)) {
            collect_render_commands_recursive(child_id, camera_pos);
        }
    }

    void Renderer::ensure_shadow_map(Entity *light_entity, const LightComponent &light) {
        if (!shadow_maps_.contains(light_entity)) {
            auto shadow_map = std::make_unique<Framebuffer>();
            FrameBufferAttachmentSettings settings;
            settings.width = 1024;
            settings.height = 1024;

            settings.min_filter = GL_NEAREST;
            settings.mag_filter = GL_NEAREST;
            settings.wrap_s = GL_CLAMP_TO_BORDER;
            settings.wrap_t = GL_CLAMP_TO_BORDER;
            shadow_map->attach_depth_texture(settings);

            glBindTexture(GL_TEXTURE_2D, shadow_map->get_depth_attachment());
            float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
            glBindTexture(GL_TEXTURE_2D, 0);
            shadow_maps_[light_entity] = {std::move(shadow_map), glm::mat4(1.0f)};
        }
    }

    void Renderer::draw_render_command(const RenderCommand &cmd, const glm::mat4 &view, const glm::mat4 &projection) {
        const Entity *entity = scene_->get_entity(cmd.entity_id);
        if (!entity) return;

        const auto *transform = entity->get_component<TransformComponent>();
        if (!transform) return;

        Shader &shader = get_shader_for_material(cmd.material);
        shader.use();

        // Upload lights
        if (context_) {
            RenderingUtils::upload_lights_to_shader(shader, *context_);

            // Bind shadow maps for directional lights
            for (int i = 0; i < context_->num_directional_lights; i++) {
                Entity* light_entity = context_->directional_light_entities[i];

                if (shadow_maps_.contains(light_entity)) {
                    auto& shadow_data = shadow_maps_[light_entity];

                    // Bind depth texture to texture unit
                    int texture_unit = 10 + i; // Start at unit 10 to avoid conflicts
                    glActiveTexture(GL_TEXTURE0 + texture_unit);
                    glBindTexture(GL_TEXTURE_2D, shadow_data.framebuffer->get_depth_attachment());

                    // Set shader uniforms
                    shader.set_int("uShadowMap[" + std::to_string(i) + "]", texture_unit);
                    shader.set_mat4("uLightSpaceMatrix[" + std::to_string(i) + "]", shadow_data.light_view_proj);
                }
            }
        }

        shader.set_vec3("uAmbientLight", scene_->environment()->get_ambient_light());

        shader.set_uint("uObjectID", cmd.entity_id);

        // Upload default uniforms
        RenderingUtils::set_standard_uniforms(shader, transform->get_world_matrix(), view, projection);

        // Bind material and draw mesh
        cmd.material->bind();
        cmd.mesh->draw();
        cmd.material->unbind();
    }

    void Renderer::draw_instanced_command(const InstancedRenderCommand &cmd, const glm::mat4 &view,
                                          const glm::mat4 &projection) {
        if (const Entity *entity = scene_->get_entity(cmd.entity_id); !entity) return;

        Shader &shader = get_shader_for_material(cmd.material);
        shader.use();

        // Upload light data as uniforms to shader
        if (context_) {
            RenderingUtils::upload_lights_to_shader(shader, *context_);
        }

        // Upload the standard uniform data to the shader (Model, View, Projection, Time)
        RenderingUtils::set_standard_uniforms(shader, glm::mat4(1.0f), view, projection, Time::current_time);

        // Prepare instanced data
        cmd.instanced_renderable->prepare_for_draw();

        // Bind material and draw
        cmd.material->bind();

        const auto mesh = cmd.instanced_renderable->get_mesh();
        if (mesh) {
            mesh->bind();
            cmd.instanced_renderable->bind_instance_buffers();
            mesh->draw_instanced(cmd.instanced_renderable->get_instance_count());
            mesh->unbind();
        }

        cmd.material->unbind();
    }

    void Renderer::execute_skybox_pass(Scene *scene, const glm::mat4 &view, const glm::mat4 &projection,
                                       CameraComponent *camera_comp) const {
        if (!scene || !scene->environment()->has_skybox()) return;

        glDisable(GL_CULL_FACE);

        if (camera_comp) {
            skybox_renderer_.render(*scene->environment()->get_skybox(), camera_comp);
        }
    }


    void Renderer::collect_lights_from_scene(Scene &scene, CameraComponent &camera) {
        const std::vector<EntityID> light_entity_ids = scene.find_entities_with_component<LightComponent>();
        std::vector<Entity *> light_entities;
        for (const EntityID id: light_entity_ids) {
            if (Entity *e = scene.get_entity(id)) {
                light_entities.push_back(e);
            }
        }
        store_lights_in_context(light_entities, camera);
    }

    void Renderer::execute_main_pass(Scene &scene, CameraComponent &camera) {
        clear_draw_list();
        scene_ = &scene;


        // Gather lights and geometry
        collect_lights_from_scene(scene, camera);
        collect_geometry_from_scene(scene, camera.get_owner().transform()->get_position());

        // Execute rendering passes
        const glm::mat4 view = camera.get_view_matrix();
        const glm::mat4 projection = camera.get_projection_matrix();

        execute_geometry_pass(view, projection);
        execute_skybox_pass(&scene, view, projection, &camera);
        execute_transparency_pass(view, projection);
    }



    void Renderer::execute_shadow_passes(Scene &scene) {
         // Gather all lights that cast shadows
        const std::vector<EntityID> light_entity_ids = scene.find_entities_with_component<LightComponent>();

        std::vector<Entity*> shadow_casting_lights;
        for (const EntityID id : light_entity_ids) {
            Entity* entity = scene.get_entity(id);
            if (!entity) continue;

            const auto* light = entity->get_component<LightComponent>();
            if (light && light->should_cast_shadows()) {
                shadow_casting_lights.push_back(entity);
            }
        }

        clear_draw_list();
        scene_ = &scene;
        const glm::vec3 dummy_camera_pos(0.0f); // Distance doesn't matter for shadows
        for (const EntityID root_id : scene.get_root_entities()) {
            collect_render_commands_recursive(root_id, dummy_camera_pos);
        }

        // Render each light's shadow map
        for (Entity* light_entity : shadow_casting_lights) {
            const auto* light = light_entity->get_component<LightComponent>();
            if (!light) continue;

            ensure_shadow_map(light_entity, *light);

            auto& shadow_data = shadow_maps_[light_entity];
            shadow_data.framebuffer->bind();

            glViewport(0, 0, 1024, 1024);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);

            // Use light's view-projection matrix
            const glm::mat4 light_view_proj = light->get_light_view_proj_matrix();
            shadow_data.light_view_proj = light_view_proj; // Store for main pass

            // Render geometry to depth texture
            draw_shadow_geometry(light_view_proj);

            shadow_data.framebuffer->unbind();

            glCullFace(GL_BACK);
        }
            glViewport(0, 0, framebuffer_width_, framebuffer_height_);            
    }

    void Renderer::draw_shadow_geometry(const glm::mat4 &light_view_proj) {
        Shader& shadow_shader = get_shader_for_material(shadow_material_);
        shadow_shader.use();
        shadow_shader.set_mat4("uLightViewProjMatrix", light_view_proj);

        shadow_material_->bind();

        for (const auto &cmd : opaque_objects_) {
            const Entity *entity = scene_->get_entity(cmd.entity_id);
            if (!entity) continue;

            const auto* transform = entity->get_component<TransformComponent>();
            if (!transform) continue;

            // Set model matrix for this object
            shadow_shader.set_mat4("uModelMatrix", transform->get_world_matrix());

            cmd.mesh->draw();
        }

        shadow_material_->unbind();
    }

    void Renderer::execute_geometry_pass(const glm::mat4 &view, const glm::mat4 &proj) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND);


        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        std::ranges::sort(opaque_objects_,
                          [](const RenderCommand &a, const RenderCommand &b) {
                              return a.distance_to_camera < b.distance_to_camera;
                          });

        for (const auto &cmd: opaque_objects_) {
            draw_render_command(cmd, view, proj);
        }

        for (const auto &cmd: opaque_instanced_objects_) {
            draw_instanced_command(cmd, view, proj);
        }
    }

    void Renderer::execute_transparency_pass(const glm::mat4 &view, const glm::mat4 &proj) {
        // Configure blending: enable for color input, disable for object ID output
        glEnablei(GL_BLEND, 0); // Enable blending for fragColor (location 0)
        glDisablei(GL_BLEND, 1); // Disable blending for objectID (location 1)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Sort the transparent objects from back-to-front relative to camera
        // This ensures proper blending order between different objects
        std::ranges::sort(transparent_objects_,
                          [](const RenderCommand &a, const RenderCommand &b) {
                              return a.distance_to_camera > b.distance_to_camera;
                          });

        // Render non-instanced transparent objects with two-pass rendering
        glDisable(GL_CULL_FACE);
        for (const auto &cmd: transparent_objects_) {
            // Pass 1: Draw back faces, to depth buffer
            glCullFace(GL_FRONT);
            glDepthMask(GL_TRUE);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(2.0f, 2.0f);
            draw_render_command(cmd, view, proj);
            glDisable(GL_POLYGON_OFFSET_FILL);

            // Pass 2: Draw front faces, don't write to depth buffer
            glCullFace(GL_BACK);
            glDepthMask(GL_FALSE);
            draw_render_command(cmd, view, proj);
        }

        // Render instanced transparent objects with two-pass rendering
        for (const auto &cmd: transparent_instanced_objects_) {
            // Pass 1: Draw back faces, to depth buffer
            glCullFace(GL_FRONT);
            glDepthMask(GL_TRUE);
            draw_instanced_command(cmd, view, proj);

            // Pass 2: Draw front faces, don't write to depth buffer
            glCullFace(GL_BACK);
            glDepthMask(GL_FALSE);
            draw_instanced_command(cmd, view, proj);
        }

        // Restore depth writing
        glDepthMask(GL_TRUE);
    }

    void Renderer::create_main_framebuffer(uint32_t width, uint32_t height) {
        framebuffer_width_ = width;
        framebuffer_height_ = height;

        // General Settings
        FrameBufferAttachmentSettings settings;
        settings.width = width;
        settings.height = height;

        // ObjectId specific settings
        FrameBufferAttachmentSettings object_id_attachment_settings = settings;
        object_id_attachment_settings.format = GL_RED_INTEGER;
        object_id_attachment_settings.internal_format = GL_R32UI;
        object_id_attachment_settings.type = GL_UNSIGNED_INT;
        scene_framebuffers_[SCREEN_TEXTURE_1] = std::make_unique<Framebuffer>();
        scene_framebuffers_[SCREEN_TEXTURE_1]->attach_color_texture(settings);
        scene_framebuffers_[SCREEN_TEXTURE_1]->attach_color_texture(object_id_attachment_settings);
        scene_framebuffers_[SCREEN_TEXTURE_1]->attach_depth_texture(settings);

        scene_framebuffers_[SCREEN_TEXTURE_2] = std::make_unique<Framebuffer>();
        scene_framebuffers_[SCREEN_TEXTURE_2]->attach_color_texture(settings);
        scene_framebuffers_[SCREEN_TEXTURE_2]->attach_color_texture(object_id_attachment_settings);
        scene_framebuffers_[SCREEN_TEXTURE_2]->attach_depth_texture(settings);
    }

    void Renderer::resize_main_framebuffer(uint32_t width, uint32_t height) {
        framebuffer_width_ = width;
        framebuffer_height_ = height;

        // Resize only the current render buffer immediately.
        // The display buffer will be lazily resized on the next frame
        // to avoid showing cleared/incomplete frames during a window resize.
        if (scene_framebuffers_[0]) {
            scene_framebuffers_[current_fb_index_]->resize(width, height);
        }
    }

    uint32_t Renderer::get_main_output_texture() const {
        const int display_index = 1 - current_fb_index_;
        if (scene_framebuffers_[display_index]) {
            return scene_framebuffers_[display_index]->get_color_attachment(0);
        }
        return 0;
    }

    uint32_t Renderer::get_object_id_texture() const {
        const int display_index = 1 - current_fb_index_;
        if (scene_framebuffers_[display_index]->get_color_attachment(1) != 0) {
            return scene_framebuffers_[display_index]->get_color_attachment(1);
        }
        return 0;
    }

    void Renderer::render_frame(Scene &scene, CameraComponent &camera) {
        if (!scene_framebuffers_[SCREEN_TEXTURE_1]) {
            create_main_framebuffer(framebuffer_width_, framebuffer_height_);
        }

        // Check if the OTHER buffer (display buffer) needs resizing
        const int display_index = 1 - current_fb_index_;
        if (scene_framebuffers_[display_index] &&
            (scene_framebuffers_[display_index]->get_width() != framebuffer_width_ ||
             scene_framebuffers_[display_index]->get_height() != framebuffer_height_)) {
            scene_framebuffers_[display_index]->resize(framebuffer_width_, framebuffer_height_);
        }

        execute_shadow_passes(scene);

        scene_framebuffers_[current_fb_index_]->bind();
        reset_framebuffer_data();

        // Clear the object ID buffer (color attachment 1) to 0
        constexpr GLuint clear_value = 0;
        glClearBufferuiv(GL_COLOR, 1, &clear_value);

        execute_main_pass(scene, camera);
        scene_framebuffers_[current_fb_index_]->unbind();
        glFlush();

        // Swap for next frame
        current_fb_index_ = 1 - current_fb_index_;
    }

    void Renderer::set_fallback_shader(Shader &fallback_shader) {
        fallback_shader_ = &fallback_shader;

        if (context_) {
            context_->shader_handle = fallback_shader.get_program_id();
        }
    }

    Shader &Renderer::get_shader_for_material(const std::shared_ptr<Material> &material) {
        if (!material) {
            return *fallback_shader_;
        }

        // Check if material has a compiled shader ID
        if (const uint32_t material_shader_id = material->get_compiled_shader_id(); material_shader_id != 0) {
            // Get shader wrapper from the ID
            if (Shader *material_shader = shader_registry_.get_shader_from_id(material_shader_id)) {
                return *material_shader;
            }
        }

        // Check if material needs compilation
        if (material->has_custom_shader()) {
            // Try to compile the material's shader
            if (const uint32_t compiled_id = compile_material_shader(material); compiled_id != 0) {
                material->set_compiled_shader_id(compiled_id);
                const auto shader = shader_registry_.get_shader_from_id(compiled_id);
                return *shader;
            }
        }

        // Fall back to default shader
        return *fallback_shader_;
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
