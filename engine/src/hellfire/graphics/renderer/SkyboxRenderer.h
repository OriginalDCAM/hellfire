//
// Created by denzel on 14/06/2025.
//

#pragma once
#include <cstdint>

#include "hellfire/ecs/CameraComponent.h"

namespace hellfire {
    class Skybox;
    class Camera;
}

namespace hellfire {
    class SkyboxRenderer {
    public:
        SkyboxRenderer() = default;
        ~SkyboxRenderer();

        void initialize();
        void render(const Skybox &skybox, const CameraComponent *camera) const;

    private:
        void setup_skybox_geometry();
        void load_skybox_shader();
        
        uint32_t skybox_shader_ = 0;
        uint32_t skybox_vao_ = 0;
        uint32_t skybox_vbo_ = 0;

        bool initialized_ = false;
    };
}
