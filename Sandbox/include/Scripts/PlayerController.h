//
// Created by denzel on 10/08/2025.
//
#pragma once
#include "DCraft/Application.h"
#include "DCraft/Components/CameraComponent.h"
#include "DCraft/Components/ScriptComponent.h"
#include "DCraft/Components/TransformComponent.h"
#include <glm/glm.hpp>
#include <iostream>

class PlayerController : public DCraft::ScriptComponent {
public:
    PlayerController(float movement_speed = 5.0f, float mouse_sensitivity = 0.1f)
        : movement_speed_(movement_speed), mouse_sensitivity_(mouse_sensitivity) {
    }

    void on_init() override {
        std::cout << "PlayerController initialized for entity: " << get_owner()->get_name() << std::endl;

        // Initialize camera orientation
        yaw_ = -90.0f;
        pitch_ = 0.0f;

        if (auto *camera = get_component<DCraft::CameraComponent>()) {
            std::cout << "Camera configured for player" << std::endl;
        }
    }

    void on_update(float delta_time) override {
        handle_movement(delta_time);
    }

    void on_remove() override {
        std::cout << "PlayerController removed from entity: " << get_owner()->get_name() << std::endl;
    }

    // Call this from your Game class mouse callback
    void handle_mouse_movement(float x_offset, float y_offset) {
        // Apply mouse sensitivity
        x_offset *= mouse_sensitivity_;
        y_offset *= mouse_sensitivity_;

        // Update yaw and pitch
        yaw_ += x_offset;
        pitch_ += y_offset;

        // Constrain pitch to prevent camera flipping
        pitch_ = glm::clamp(pitch_, -89.0f, 89.0f);

        // Update camera orientation directly
        update_camera_orientation();
    }

    // Setters and getters
    void set_movement_speed(float speed) { movement_speed_ = speed; }
    float get_movement_speed() const { return movement_speed_; }

    void set_mouse_sensitivity(float sensitivity) {
        mouse_sensitivity_ = sensitivity;
    }

    float get_mouse_sensitivity() const { return mouse_sensitivity_; }

    // Camera orientation getters
    float get_yaw() const { return yaw_; }
    float get_pitch() const { return pitch_; }

private:
    float movement_speed_;
    float mouse_sensitivity_;
    // Camera orientation 
    float yaw_ = -90.0f;
    float pitch_ = 0.0f;

    void update_camera_orientation() {
        auto *camera = get_component<DCraft::CameraComponent>();
        if (!camera) return;

        // Calculate camera direction vectors from yaw and pitch
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front = glm::normalize(front);

        // Update camera's internal vectors by setting yaw/pitch directly
        camera->set_yaw(yaw_);
        camera->set_pitch(pitch_);
    }

    void handle_movement(float delta_time) {
        auto &app = DCraft::Application::get_instance();
        auto *camera = get_component<DCraft::CameraComponent>();
        if (!camera) return;

        // Calculate movement direction based on camera orientation
        glm::vec3 movement_direction(0.0f);

        if (app.is_key_pressed('w') || app.is_key_pressed('W')) {
            movement_direction += glm::vec3(camera->get_front_vector().x, 0, camera->get_front_vector().z);
        }
        if (app.is_key_pressed('s') || app.is_key_pressed('S')) {
            movement_direction -= glm::vec3(camera->get_front_vector().x, 0, camera->get_front_vector().z);
        }
        if (app.is_key_pressed('a') || app.is_key_pressed('A')) {
            movement_direction -= camera->get_right_vector();
        }
        if (app.is_key_pressed('d') || app.is_key_pressed('D')) {
            movement_direction += camera->get_right_vector();
        }
        if (app.is_key_pressed('e') || app.is_key_pressed('E')) {
            movement_direction -= glm::vec3(0, camera->get_up_vector().y, 0);
        }
        if (app.is_key_pressed('q') || app.is_key_pressed('Q')) {
            movement_direction += glm::vec3(0, camera->get_up_vector().y, 0);
        }

        // Apply movement speed and sprint modifier
        float current_speed = movement_speed_;
        if (app.is_key_pressed(16)) {
            current_speed *= 2.0f;
        }

        // Normalize movement direction to prevent faster diagonal movement
        if (glm::length(movement_direction) > 0.0f) {
            movement_direction = glm::normalize(movement_direction);
        }

        // Apply movement to transform
        DCraft::TransformComponent *transform = get_transform();
        if (transform) {
            glm::vec3 current_position = transform->get_position();
            glm::vec3 new_position = current_position + movement_direction * current_speed * delta_time;
            transform->set_position(new_position.x, new_position.y, new_position.z);
            camera->invalidate_view();
        }
    }

    void on_interact() {
        std::cout << "Player interacted!" << std::endl;

        
    }

};
