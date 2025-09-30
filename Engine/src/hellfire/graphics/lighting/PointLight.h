//
// Created by denzel on 08/08/2025.
//

#pragma once
#include <string>

#include "hellfire/ecs/LightComponent.h"

namespace hellfire {
    class Scene;
    using EntityID = uint32_t;

    class PointLight {
    public:
        static EntityID create(
            Scene* scene,
            const std::string& name = "PointLight",
            const glm::vec3& position = glm::vec3(0.0f),
            const glm::vec3& color = glm::vec3(1.0f),
            float intensity = 1.0f,
            float range = 10.0f
        );
    };
}
