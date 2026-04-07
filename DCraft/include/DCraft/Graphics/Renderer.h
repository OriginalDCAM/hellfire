#pragma once
#include <cstdint>
#include "DCraft/Structs/Object3D.h"
#include "DCraft/Structs/Camera.h"
#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"


namespace DCraft
{

    class OGLRendererContext {
    public:
        uint32_t default_shader_program;
    };

    class Renderer
    {
    public:
        Renderer(uint32_t program_id_);
        ~Renderer();

        void render(Object3D& scene, Camera& camera);

        uint32_t get_default_shader() const;
        void* get_context() const;
        
        void begin_frame();
        void end_frame();
    private:
        uint32_t fallback_program_id_;
        void* context_;
        int32_t uniform_mvp_;
        
        void collect_lights(Object3D* object_3d, std::vector<DirectionalLight*>& dir_lights, std::vector<PointLight*>& point_lights);

        void collect_unique_shaders(Object3D *object, std::unordered_set<uint32_t> &shaders);

        void upload_lights_to_shader(uint32_t shader_program, const std::vector<DirectionalLight *> &dir_lights,
                                     const std::vector<PointLight *> &point_lights);

        void upload_global_uniforms_to_shader(uint32_t shader_program, Camera &camera);

        void upload_lights(uint32_t shader_program, const std::vector<DirectionalLight*>& dir_lights, const std::vector<PointLight*>& point_lights);
    };
}

