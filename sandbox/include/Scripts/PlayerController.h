//
// Created by denzel on 10/08/2025.
//
#pragma once
#include "hellfire/ecs/ScriptComponent.h"
#include <glm/detail/type_vec3.hpp>

class SceneCameraScript final : public hellfire::ScriptComponent {
public:
    explicit SceneCameraScript(float movement_speed = 5.0f, float mouse_sensitivity = 0.1f);

    void on_init() override;
    void handle_drone_mode(float delta_time);

    void save_free_flight_position();

    void restore_free_flight_position();

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

    bool drone_mode_ = true; // Start in drone mode

    float saved_yaw_ = -90.0f;
    float saved_pitch_ = 0.0f;

    glm::vec3 last_known_player_position = glm::vec3(0.0f);
    glm::vec3 drone_position = glm::vec3(100.0f, 100.0f, 100.0f);

    void update_camera_orientation() const;
    void handle_movement(float delta_time) const;
    void set_drone_overview_position(); 
    void on_interact();
};