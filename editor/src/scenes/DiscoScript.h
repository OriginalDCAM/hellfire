//
// Created by denzel on 01/04/2026.
//

#pragma once
#include "hellfire/ecs/LightComponent.h"
#include "hellfire/ecs/ScriptComponent.h"

class DiscoScript : public hellfire::ScriptComponent {
    SCRIPT_CLASS(DiscoScript);
    
    SCRIPT_VAR(bool, should_play, true);
    SCRIPT_VAR(glm::vec3, play_speed, glm::vec3(0.25));

    void on_init() override {
        REGISTER_VAR(should_play, BOOL);
        REGISTER_VAR(play_speed, VEC3);
    }

    void on_update(const float delta_time) override {
        if (should_play) {
            if (auto light_comp = get_owner().get_component<hellfire::LightComponent>()) {
                auto current_color = light_comp->get_color();
                current_color += play_speed * delta_time;
                light_comp->set_color(glm::mod(current_color, glm::vec3(1.0f)));
            }
        }
    }
};
