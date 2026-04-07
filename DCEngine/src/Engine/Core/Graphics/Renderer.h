#pragma once
#include <cstdint>
#include "../Object3D.h"
#include "../../Cameras/Camera.h"


namespace DCraft
{

    class Renderer
    {
        uint32_t program_id_;
        int32_t uniform_mvp_;
    public:
        Renderer(uint32_t program_id_);

        void render(Object3D& scene, Camera& camera);
        void begin_frame();
        void end_frame();

    };
}

