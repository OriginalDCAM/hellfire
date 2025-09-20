//
// Created by denzel on 08/08/2025.
//

#pragma once
#include "DCraft/Components/LightComponent.h"
#include "DCraft/Structs/Entity.h"

namespace hellfire {
    class SpotLight {
    public:
        static Entity *create(const std::string &name = "SpotLight",
                              const glm::vec3 &position = glm::vec3(0.0f),
                              const glm::vec3 &direction = glm::vec3(0.0f, -1.0f, 0.0f),
                              const glm::vec3 &color = glm::vec3(1.0f),
                              float intensity = 1.0f,
                              float range = 10.0f,
                              float inner_angle = 30.0f,
                              float outer_angle = 45.0f) {
            Entity *entity = new Entity(name);
            auto *light = entity->add_component<LightComponent>(LightComponent::LightType::SPOT);

            light->set_direction(direction);
            light->set_color(color);
            light->set_intensity(intensity);
            light->set_range(range);
            light->set_inner_cone_angle(inner_angle);
            light->set_outer_cone_angle(outer_angle);

            entity->transform()->set_position(position);

            return entity;
        }
    };
}
