#pragma once
#include <cstdint>
#include <vector>
#include <memory>

#include "RendererContext.h"
#include "hellfire/ecs/Entity.h"
#include "hellfire/ecs/LightComponent.h"
#include "hellfire/ecs/RenderableComponent.h"
#include "hellfire/graphics/backends/opengl/Framebuffer.h"
#include "hellfire/graphics/renderer/SkyboxRenderer.h"
#include "hellfire/graphics/shader/ShaderRegistry.h"

namespace hellfire {
    class InstancedRenderableComponent;
    class Scene;
    class Material;
    class Mesh;

    using EntityID = uint32_t;

    struct RenderCommand {
        EntityID entity_id; // The entity being rendered
        std::shared_ptr<Mesh> mesh; // Direct reference to renderable component
        std::shared_ptr<Material> material; // Material for sorting and rendering
        float distance_to_camera; // Distance for sorting
        bool is_transparent; // Transparency flag for render pass

        bool operator<(const RenderCommand &other) const {
            if (is_transparent && other.is_transparent) {
                return distance_to_camera > other.distance_to_camera;
            }
            return !is_transparent && other.is_transparent;
        }
    };

    struct InstancedRenderCommand {
        EntityID entity_id;
        InstancedRenderableComponent *instanced_renderable;
        std::shared_ptr<Material> material;
        float distance_to_camera;
        bool is_transparent;

        bool operator<(const RenderCommand &other) const {
            if (is_transparent && other.is_transparent) {
                return distance_to_camera > other.distance_to_camera;
            }
            return !is_transparent && other.is_transparent;
        }
    };

    class Renderer {
    public:
        Renderer();

        ~Renderer() = default;

        void init();

        void render(Scene &scene, const Entity *camera_override);

        void render_frame(Scene &scene, CameraComponent &camera);

        void reset_framebuffer_data();

        void clear_draw_list();

        void begin_frame();

        void end_frame();

        [[nodiscard]] Shader *get_fallback_shader() const {
            return fallback_shader_;
        }

        // Framebuffer management
        void create_main_framebuffer(uint32_t width, uint32_t height);

        void set_render_to_framebuffer(bool enable) { render_to_framebuffer_ = enable; }

        uint32_t get_main_output_texture() const;

        uint32_t get_object_id_texture() const;

        void resize_main_framebuffer(uint32_t width, uint32_t height);

        void set_fallback_shader(Shader &fallback_shader);

        Shader &get_shader_for_material(const std::shared_ptr<Material> &material);

        uint32_t compile_material_shader(std::shared_ptr<Material> material);

        ShaderManager &get_shader_manager() { return shader_manager_; }
        ShaderRegistry &get_shader_registry() { return shader_registry_; }

    private:
        enum RendererFboId : uint32_t {
            SCREEN_TEXTURE_1 = 0,
            SCREEN_TEXTURE_2 = 1,
            SHADOW_MAP = 3
        };

        ShaderManager shader_manager_;
        ShaderRegistry shader_registry_;

        Shader *fallback_shader_;
        uint32_t fallback_program_;
        std::unique_ptr<OGLRendererContext> context_;
        Scene *scene_; // Current scene being rendered

        std::unique_ptr<Framebuffer> scene_framebuffers_[2];
        int current_fb_index_ = 0;

        bool render_to_framebuffer_;
        uint32_t framebuffer_width_;
        uint32_t framebuffer_height_;

        // Render command lists
        std::vector<RenderCommand> opaque_objects_;
        std::vector<RenderCommand> transparent_objects_;
        std::vector<InstancedRenderCommand> opaque_instanced_objects_;
        std::vector<InstancedRenderCommand> transparent_instanced_objects_;
        std::unordered_map<Entity *, ShadowMapData> shadow_maps_;

        SkyboxRenderer skybox_renderer_;
        std::shared_ptr<Material> shadow_material_;

        void collect_render_commands_recursive(EntityID entity_id, const glm::vec3 &camera_pos);

        void ensure_shadow_map(Entity *light_entity, const LightComponent &light);

        void store_lights_in_context(const std::vector<Entity *> &light_entities, CameraComponent &camera);

        void collect_lights_from_scene(Scene & scene, CameraComponent & camera);
        void collect_geometry_from_scene(Scene &scene, const glm::vec3 camera_pos);

        void execute_main_pass(Scene& scene, CameraComponent& camera);

        void draw_shadow_geometry(const glm::mat4& light_view_proj);

        void execute_shadow_passes(Scene& scene);
        void execute_geometry_pass(const glm::mat4 &view, const glm::mat4 &proj);
        void execute_skybox_pass(Scene *scene, const glm::mat4 &view, const glm::mat4 &projection,
                                CameraComponent *camera_comp) const;
        void execute_transparency_pass(const glm::mat4 &view, const glm::mat4 &proj);

        // Draw methods
        void draw_render_command(const RenderCommand &cmd, const glm::mat4 &view, const glm::mat4 &projection);

        void draw_instanced_command(const InstancedRenderCommand &cmd, const glm::mat4 &view,
                                    const glm::mat4 &projection);
    };
}
