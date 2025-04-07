//
// Created by denzel on 07/04/2025.
//

#pragma once
#include <glm/glm.hpp>

#include "DCraft/Graphics/OGL/StandardMaterial.h"

namespace DCraft {
    class LambertMaterial : public OGL::StandardMaterial {
    public:
        LambertMaterial(const std::string& name) : StandardMaterial(name) {}

        void set_ambient_color(const glm::vec3& color) { ambient_color_ = color; }
        void set_diffuse_color(const glm::vec3& color) { diffuse_color_ = color; }

        void bind(void* renderer_context) override;
    private:
        glm::vec3 ambient_color_ = glm::vec3(0.1f);
        glm::vec3 diffuse_color_ = glm::vec3(1.0f);
    };
}
