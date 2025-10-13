//
// Created by denzel on 07/10/2025.
//

#pragma once
#include "glm/glm.hpp"
#include "hellfire/ecs/ScriptComponent.h"
#include "hellfire/ecs/TransformComponent.h"

class RotateScript : public hellfire::ScriptComponent {
public:
    void on_init() override {
        
    }

    void on_update(float delta_time) override {
        auto current_rotation = get_transform()->get_rotation();
        current_rotation = current_rotation + glm::vec3(45, 45, 0) * delta_time;
        get_transform()->set_rotation(current_rotation);
    }
};
