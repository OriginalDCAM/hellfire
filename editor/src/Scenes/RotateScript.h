//
// Created by denzel on 07/10/2025.
//

#pragma once
#include "hellfire/ecs/ScriptComponent.h"
#include "hellfire/ecs/TransformComponent.h"


class RotateScript : public hellfire::ScriptComponent {
    SCRIPT_CLASS(RotateScript);
    
    SCRIPT_VAR(bool, should_rotate, true);
    SCRIPT_VAR(glm::vec3, rotation_speed, glm::vec3(45.0f, 45.0f, 0.0f));

    void on_init() override {
        REGISTER_VAR(should_rotate, BOOL);
        REGISTER_VAR(rotation_speed, VEC3);
    }

    void on_update(const float delta_time) override {
        if (should_rotate) {
            auto current = get_transform()->get_rotation();
            current += rotation_speed * delta_time;
            get_transform()->set_rotation(mod(current, glm::vec3(360.0f)));
        }
    }
};
