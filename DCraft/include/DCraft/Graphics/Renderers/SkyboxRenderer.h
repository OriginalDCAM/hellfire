//
// Created by denzel on 14/06/2025.
//

#pragma once
#include <cstdint>

#include "DCraft/Components/CameraComponent.h"

namespace DCraft {
    class Skybox;
    class Camera;
}

namespace DCraft {
    class SkyboxRenderer {
    public:
        SkyboxRenderer() = default;
        ~SkyboxRenderer();

        void initialize();
        void render(const Skybox *skybox, const CameraComponent *camera) const;

    private:
        void setup_skybox_geometry();
        void load_skybox_shader();
        
        uint32_t skybox_shader_ = 0;
        uint32_t skybox_vao_ = 0;
        uint32_t skybox_vbo_ = 0;

        bool initialized_ = false;
    };
}
