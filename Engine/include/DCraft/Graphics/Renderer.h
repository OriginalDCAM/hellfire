#pragma once
#include <cstdint>
#include <vector>
#include <memory>

#include "DCraft/Structs/Entity.h"
#include "DCraft/Components/CameraComponent.h"
#include "DCraft/Components/RenderableComponent.h"
#include "DCraft/Graphics/OGL/Framebuffer.h"
#include "DCraft/Graphics/Renderers/SkyboxRenderer.h"
#include "Shader/ShaderRegistry.h"

namespace hellfire {
    class InstancedRenderableComponent;
}

namespace hellfire {
    class Scene;

    struct RenderCommand {
        Entity* entity;                    // The entity being rendered
        RenderableComponent* renderable;   // Direct reference to renderable component
        std::shared_ptr<Material> material;                // Material for sorting and rendering
        float distance_to_camera;          // Distance for sorting
        bool is_transparent;               // Transparency flag for render pass

        bool operator<(const RenderCommand& other) const {
            if (is_transparent && other.is_transparent) {
                return distance_to_camera > other.distance_to_camera;
            }
            return !is_transparent && other.is_transparent;
        }
    };

    struct InstancedRenderCommand {
        Entity* entity;
        InstancedRenderableComponent* instanced_renderable;
        std::shared_ptr<Material> material;
        float distance_to_camera;
        bool is_transparent;

        bool operator<(const RenderCommand& other) const {
            if (is_transparent && other.is_transparent) {
                return distance_to_camera > other.distance_to_camera;
            }
            return !is_transparent && other.is_transparent;
        }
    };

    class Renderer {


    public:
        Renderer(uint32_t fallback_program_id);
        Renderer();
        ~Renderer();
        
        void init();
        
        void render(Scene& scene, float time = 0.0f);

        void begin_frame();
        void end_frame();

        [[nodiscard]] Shader *get_default_shader() const;
        [[nodiscard]] void* get_context() const;

        // Framebuffer management
        void create_scene_framebuffer(uint32_t width, uint32_t height);
        void set_render_to_framebuffer(bool enable) { render_to_framebuffer_ = enable; }
        uint32_t get_scene_texture() const;
        void resize_scene_framebuffer(uint32_t width, uint32_t height);

        void render_to_texture(Scene& scene, CameraComponent& camera, uint32_t width, uint32_t height);
        void render_scene_to_framebuffer(Scene& scene, CameraComponent& camera);

        void set_fallback_shader(Shader &fallback_shader);

        Shader *get_shader_for_material(std::shared_ptr<Material> material);

        uint32_t compile_material_shader(std::shared_ptr<Material> material);

        ShaderManager& get_shader_manager() { return shader_manager_; }
        ShaderRegistry& get_shader_registry() { return shader_registry_; }
        
    private:
        ShaderManager shader_manager_;
        ShaderRegistry shader_registry_;
        
        Shader* fallback_shader_;
        uint32_t fallback_program_;
        void* context_;

        std::unique_ptr<Framebuffer> scene_framebuffer_;
        bool render_to_framebuffer_;
        uint32_t framebuffer_width_;
        uint32_t framebuffer_height_;
        
        // Render command lists
        std::vector<RenderCommand> opaque_objects_;
        std::vector<RenderCommand> transparent_objects_;

        // Instanced Render command lists
        std::vector<InstancedRenderCommand> opaque_instanced_objects_;
        std::vector<InstancedRenderCommand> transparent_instanced_objects_;

        SkyboxRenderer skybox_renderer_;
        
        void collect_render_commands_recursive(Entity* entity, const glm::vec3& camera_pos);
        void store_lights_in_context(const std::vector<Entity*>& light_entities, CameraComponent& camera) const;
        void render_internal(Scene& scene, CameraComponent& camera);
        
        // Rendering passes
        void render_opaque_pass(const glm::mat4& view, const glm::mat4& projection);
        void render_transparent_pass(const glm::mat4& view, const glm::mat4& projection);
        void render_skybox_pass(Scene* scene, const glm::mat4& view, const glm::mat4& projection) const;

        // Utility methods
        bool is_material_transparent(const std::shared_ptr<Material> &material);
        
    };
}
