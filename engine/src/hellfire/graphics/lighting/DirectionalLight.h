//
// Created by denzel on 08/08/2025.
//

#pragma once
#include "hellfire/ecs/LightComponent.h"

namespace hellfire {
    class Scene;
    using EntityID = uint32_t;

    class DirectionalLight {
    public:
        static EntityID create(
            Scene* scene,
            const std::string& name = "DirectionalLight", 
            const glm::vec3& direction = glm::vec3(0.0f, -1.0f, 0.0f),
            const glm::vec3& color = glm::vec3(1.0f),
            float intensity = 1.0f
        );
    };
}
