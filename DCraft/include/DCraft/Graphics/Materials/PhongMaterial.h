//
// Created by denzel on 07/04/2025.
//

#pragma once
#include <glm/glm.hpp>

#include "DCraft/Graphics/OGL/StandardMaterial.h"

namespace DCraft {
    class PhongMaterial : public OGL::StandardMaterial {
    public:
        PhongMaterial();
        explicit PhongMaterial(const std::string &name) : StandardMaterial(name) {
        }

        void set_specular_color(const glm::vec3 &color) { specular_color_ = color; }
        void set_shininess(float value) { shininess_ = value; }

        void bind(void *renderer_context) override;

        glm::vec3 get_specular_color() const { return specular_color_; }

        float get_shininess() const { return shininess_; };

    private:
        glm::vec3 ambient_color_ = glm::vec3(0.1f);
        glm::vec3 diffuse_color_ = glm::vec3(1.0f);
        glm::vec3 specular_color_ = glm::vec3(0.5f);
        float shininess_ = 32.0f;
    };
}
