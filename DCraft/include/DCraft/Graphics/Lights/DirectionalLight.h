//
// Created by denzel on 07/04/2025.
//

#pragma once
#include "Light.h"

namespace DCraft {
    class DirectionalLight : public Light {
    public:
        DirectionalLight() = default;
        DirectionalLight(const std::string &name = "DirectionalLight");

        void set_direction(const glm::vec3 &dir);

        const glm::vec3 &get_direction() const { return direction_; }

        void look_at(const glm::vec3 &target);

        void upload_to_shader(uint32_t shader_program, int light_index) override;

        json to_json() override;

    private:
        glm::vec3 direction_ = glm::vec3(0.0f, -1.0f, 0.0f);
    };
}
