//
// Created by denzel on 14/06/2025.
//

#pragma once
#include <complex.h>
#include <cstdint>

#include "hellfire/ecs/CameraComponent.h"
#include "hellfire/graphics/Mesh.h"
#include "hellfire/graphics/shader/Shader.h"

namespace hellfire {
    class Skybox;
    class Camera;
}

namespace hellfire {
    class SkyboxRenderer {
    public:
        SkyboxRenderer() = default;
        ~SkyboxRenderer() = default;

        void initialize();
        void render(const Skybox &skybox, const CameraComponent *camera) const;

    private:
        void setup_skybox_geometry();
        void load_skybox_shader();
        
        uint32_t skybox_shader_id_ = 0;
        bool initialized_ = false;
        Shader skybox_shader_;
        std::shared_ptr<Mesh> skybox_mesh_;
    };
}
