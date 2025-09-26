//
// Created by denzel on 08/08/2025.
//

#pragma once
#include <string>

#include "hellfire/ecs/LightComponent.h"

namespace hellfire {
    class PointLight {
    public:
        static Entity* create(const std::string& name = "PointLight",
                             const glm::vec3& position = glm::vec3(0.0f),
                             const glm::vec3& color = glm::vec3(1.0f),
                             float intensity = 1.0f,
                             float range = 10.0f,
                             float attenuation = 1.0f) {
            
            Entity* entity = new Entity(name);
            auto* light = entity->add_component<LightComponent>(LightComponent::LightType::POINT);
            
            light->set_color(color);
            light->set_intensity(intensity);
            light->set_range(range);
            light->set_attenuation(attenuation);
            
            entity->transform()->set_position(position);
            
            return entity;
        }
    };
}
