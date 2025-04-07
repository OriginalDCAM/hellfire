//
// Created by denzel on 07/04/2025.
//

#pragma once
#include "DCraft/Structs/Object3D.h"

namespace DCraft {
    class Light : public Object3D {
    protected:
        glm::vec3 color = glm::vec3(1.0f);
        float intensity = 1.0f;
    public:
        Light(const std::string& name = "Light");

        void set_color(const glm::vec3& light_color) { color = light_color; }
        const glm::vec3& get_color() const { return color; }

        void set_intensity(float value) { intensity = value; }
        float get_intensity() const { return intensity; }

        virtual void upload_to_shader(uint32_t shader_program, int light_index) = 0;
    };
}
