//
// Created by denzel on 07/04/2025.
//

#include "DCraft/Graphics/Lights/DirectionalLight.h"

#include <glm/gtc/type_ptr.inl>

namespace DCraft {
    DirectionalLight::DirectionalLight(const std::string &name) : Light(name) {
    }

    void DirectionalLight::set_direction(const glm::vec3 &dir) {
        direction_ = glm::normalize(dir);
    }

    void DirectionalLight::look_at(const glm::vec3 &target) {
        glm::vec3 position = get_world_position();
        glm::vec3 new_dir = glm::normalize(target - position);
        set_direction(new_dir);
    }

    void DirectionalLight::upload_to_shader(uint32_t shader_program, int light_index) {
        std::string base = "directionalLights[" + std::to_string(light_index) + "]";

        glUniform3fv(glGetUniformLocation(shader_program, (base + ".direction").c_str()), 1,
                     glm::value_ptr(get_direction()));
        glUniform3fv(glGetUniformLocation(shader_program, (base + ".color").c_str()), 1,
                     glm::value_ptr(get_color()));
        glUniform1f(glGetUniformLocation(shader_program, (base + ".intensity").c_str()), get_intensity());
    }

    json DirectionalLight::to_json() {
        json j = Light::to_json();
        j["light_type"] = "DirectionalLight";
        j["direction"] = { direction_.x, direction_.y, direction_.z };
        return j;
    }
}
