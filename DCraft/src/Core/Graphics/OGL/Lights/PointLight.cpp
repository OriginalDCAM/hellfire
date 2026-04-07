//
// Created by denzel on 07/04/2025.
//

#include "DCraft/Graphics/Lights/PointLight.h"

#include <glm/gtc/type_ptr.inl>

namespace DCraft {
    PointLight::PointLight(const std::string &name) : Light(name) {
    }

    void PointLight::upload_to_shader(uint32_t shader_program, int light_index) {
        std::string base = "pointLights[" + std::to_string(light_index) + "]";
        
        // Use world position to account for hierarchy transforms
        glm::vec3 position = get_world_position();
        
        glUniform3fv(glGetUniformLocation(shader_program, (base + ".position").c_str()), 1, 
                     glm::value_ptr(position));
        glUniform3fv(glGetUniformLocation(shader_program, (base + ".color").c_str()), 1, 
                     glm::value_ptr(color));
        glUniform1f(glGetUniformLocation(shader_program, (base + ".intensity").c_str()), intensity);
        glUniform1f(glGetUniformLocation(shader_program, (base + ".range").c_str()), range);
        glUniform1f(glGetUniformLocation(shader_program, (base + ".attenuation").c_str()), attenuation);
    }

    json PointLight::to_json() {
        json j = Light::to_json();
        j["light_type"] = "PointLight";
        j["range"] = range;
        j["attenuation"] = attenuation;
        return j;
    }
}
