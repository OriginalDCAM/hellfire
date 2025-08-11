#pragma once
#include <cstdint>
#include <vector>
#include <memory>

#include "DCraft/Structs/Entity.h"
#include "DCraft/Components/CameraComponent.h"
#include "DCraft/Components/RenderableComponent.h"
#include "DCraft/Graphics/RendererContext.h"  // Include the separate context header
#include "DCraft/Graphics/Materials/Material.h"
#include "DCraft/Graphics/OGL/Framebuffer.h"
#include "DCraft/Graphics/Renderers/SkyboxRenderer.h"

namespace DCraft {
    class Scene;

    struct RenderCommand {
        Entity* entity;                    // The entity being rendered
        RenderableComponent* renderable;   // Direct reference to renderable component
        Material* material;                // Material for sorting and rendering
        float distance_to_camera;          // Distance for sorting
        bool is_transparent;               // Transparency flag for render pass

        bool operator<(const RenderCommand& other) const {
            if (is_transparent && other.is_transparent) {
                return distance_to_camera > other.distance_to_camera;
            }
            return !is_transparent && other.is_transparent;
        }
    };

    class Renderer {
    private:
        uint32_t fallback_program_id_;
        void* context_;

        std::unique_ptr<Framebuffer> scene_framebuffer_;
        bool render_to_framebuffer_;
        uint32_t framebuffer_width_;
        uint32_t framebuffer_height_;
        
        // Render command lists
        std::vector<RenderCommand> opaque_objects_;
        std::vector<RenderCommand> transparent_objects_;

        SkyboxRenderer skybox_renderer_;

    public:
        Renderer(uint32_t fallback_program_id);
        Renderer();
        ~Renderer();
        
        void init();
        
        // Main render method - now takes Scene instead of Object3D
        void render(Scene& scene);

        void begin_frame();
        void end_frame();
        
        uint32_t get_default_shader() const;
        void* get_context() const;

        // Framebuffer management
        void create_scene_framebuffer(uint32_t width, uint32_t height);
        void set_render_to_framebuffer(bool enable) { render_to_framebuffer_ = enable; }
        uint32_t get_scene_texture() const;
        void resize_scene_framebuffer(uint32_t width, uint32_t height);

        // Updated to use Scene and CameraComponent
        void render_to_texture(Scene& scene, CameraComponent& camera, uint32_t width, uint32_t height);
        void render_scene_to_framebuffer(Scene& scene, CameraComponent& camera);

        void set_fallback_shader(uint32_t shader_program_id);

    private:
        // Updated collection methods for Entity-Component system
        void collect_render_commands_recursive(Entity* entity, const glm::vec3& camera_pos);
        
        // Updated light storage for Entity-based lights
        void store_lights_in_context(const std::vector<Entity*>& light_entities, CameraComponent& camera);

        // Updated to use Scene and CameraComponent
        void render_internal(Scene& scene, CameraComponent& camera);
        
        // Rendering passes
        void render_opaque_pass(const glm::mat4& view, const glm::mat4& projection);
        void render_transparent_pass(const glm::mat4& view, const glm::mat4& projection);
        void render_skybox_pass(Scene* scene, const glm::mat4& view, const glm::mat4& projection) const;

        // Utility methods
        bool is_material_transparent(Material* material);
    };
}