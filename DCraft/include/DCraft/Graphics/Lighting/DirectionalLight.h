//
// Created by denzel on 08/08/2025.
//

#pragma once
#include "DCraft/Components/LightComponent.h"
#include "DCraft/Structs/Entity.h"

namespace DCraft {
    class DirectionalLight {
    public:
        static Entity* create(const std::string& name = "DirectionalLight", 
                     const glm::vec3& direction = glm::vec3(0.0f, -1.0f, 0.0f),
                     const glm::vec3& color = glm::vec3(1.0f),
                     float intensity = 1.0f) {
            
            Entity* entity = new Entity(name);
            auto* light = entity->add_component<LightComponent>(LightComponent::LightType::DIRECTIONAL);
            
            light->set_direction(direction);
            light->set_color(color);
            light->set_intensity(intensity);
            
            return entity;
        }
    };
}
