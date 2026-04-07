//
// Created by denzel on 10/08/2025.
//
#pragma once
#include "hellfire/ecs/ScriptComponent.h"
#include <glm/detail/type_vec3.hpp>

class SceneCameraScript final : public hellfire::ScriptComponent {
    SCRIPT_CLASS(SceneCameraScript);
    explicit SceneCameraScript(float movement_speed = 5.0f, float mouse_sensitivity = 0.1f);

    SCRIPT_VAR(float, look_speed, 45.0f);

    void on_init() override;

    void handle_keyboard_look(float delta_time);
    void on_update(float delta_time) override;
    void on_remove() override;
    void handle_mouse_movement(float x_offset, float y_offset);

    // Setters and getters
    void set_movement_speed(float speed) { movement_speed_ = speed; }
    [[nodiscard]] float get_movement_speed() const { return movement_speed_; }
    void set_mouse_sensitivity(float sensitivity) { mouse_sensitivity_ = sensitivity; }
    [[nodiscard]] float get_mouse_sensitivity() const { return mouse_sensitivity_; }

    // Camera orientation getters
    [[nodiscard]] float get_yaw() const { return yaw_; }
    [[nodiscard]] float get_pitch() const { return pitch_; }

private:
    float movement_speed_;
    float mouse_sensitivity_;
    
    // Camera orientation 
    float yaw_ = -90.0f;
    float pitch_ = 0.0f;

    void update_camera_orientation() const;
    void handle_movement(float delta_time) const;
    void on_interact();
};