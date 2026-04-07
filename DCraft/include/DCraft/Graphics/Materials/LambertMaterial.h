//
// Created by denzel on 07/04/2025.
//

#pragma once
#include <glm/glm.hpp>

#include "DCraft/Graphics/OGL/StandardMaterial.h"

namespace DCraft {
    class LambertMaterial : public OGL::StandardMaterial {
    public:
        LambertMaterial();
        LambertMaterial(const std::string& name) : StandardMaterial(name) {}
        void bind(void* renderer_context) override;
    private:


        void on_diffuse_texture_set(Texture *texture) override { has_diffuse_texture_ = true; }

    };
}
