//
// Created by denzel on 07/04/2025.
//

#pragma once
#include "Light.h"

namespace DCraft {
    class PointLight : public Light {
    public:
        PointLight(const std::string& name = "PointLight");

        void set_range(float r) { range = r; }
        float get_range() const { return range; }

        void set_attenuation(float att) { attenuation = att; }
        float get_attenuation() const { return attenuation; }

        void upload_to_shader(uint32_t shader_program, int light_index) override;
        
    private:
        float range = 10.0f;
        float attenuation = 1.0f;
    };
}
