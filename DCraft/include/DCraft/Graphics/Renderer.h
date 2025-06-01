#pragma once
#include <cstdint>
#include "DCraft/Structs/Object3D.h"
#include "DCraft/Structs/Camera.h"
#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"
#include "OGL/Framebuffer.h"


namespace DCraft
{
    struct RenderCommand {
        Object3D* object;
        Material* material;
        float distance_to_camera;
        bool is_transparent;

        bool operator<(const RenderCommand& other) const {
            if (is_transparent && other.is_transparent) {
                return distance_to_camera > other.distance_to_camera;
            }
            return !is_transparent && other.is_transparent;
        }
    };
    class OGLRendererContext {
    public:
        uint32_t default_shader_program;
    
        // Fixed arrays matching shader limits
        DirectionalLight* directional_lights[4];  // MAX_DIRECTIONAL_LIGHTS = 4
        PointLight* point_lights[8];              // MAX_POINT_LIGHTS = 8
        int num_directional_lights = 0;
        int num_point_lights = 0;
    
        Camera* camera = nullptr;
    
        // Constructor to initialize arrays
        OGLRendererContext() : default_shader_program(0) {
            for (int i = 0; i < 4; i++) directional_lights[i] = nullptr;
            for (int i = 0; i < 8; i++) point_lights[i] = nullptr;
        }
    };

    class Renderer
    {
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
        
    public:
        Renderer(uint32_t fallback_program_id);
        Renderer();
 
        ~Renderer();
        
        void init();
        void render(Object3D &scene, Camera &camera);
        void begin_frame();
        void end_frame();
        
        uint32_t get_default_shader() const;
        void *get_context() const;

        // Framebuffer management
        void create_scene_framebuffer(uint32_t width, uint32_t height);
        void set_render_to_framebuffer(bool enable) { render_to_framebuffer_ = enable; }
        uint32_t get_scene_texture() const;
        void resize_scene_framebuffer(uint32_t width, uint32_t height);

        void render_to_texture(Object3D &scene, Camera &camera, uint32_t width, uint32_t height);
        void render_scene_to_framebuffer(Object3D &scene, Camera &camera);

        void set_fallback_shader(uint32_t shader_program_id);

    private:
        // Collection methods
        void collect_render_commands(Object3D *object, const glm::vec3& camera_pos);
        void collect_lights(Object3D *object_3d, std::vector<DirectionalLight *> &dir_lights,
                           std::vector<PointLight *> &point_lights);

        // Data storage methods
        void store_lights_in_context(const std::vector<DirectionalLight *> &dir_lights,
                                     const std::vector<PointLight *> &point_lights, Camera &camera);

        void render_internal(Object3D& scene, Camera &camera);
        // Rendering passes
        void render_shadow_pass(const glm::mat4& light_view, const glm::mat4& light_projection);
        void render_opaque_pass(const glm::mat4& view, const glm::mat4& projection);
        void render_transparent_pass(const glm::mat4& view, const glm::mat4& projection);

        // Utility methods
        bool is_material_transparent(Material *material);
    };
}

