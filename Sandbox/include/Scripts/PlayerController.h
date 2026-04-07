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
    PlayerController(float movement_speed = 5.0f, float mouse_sensitivity = 0.1f, bool enable_physics = false)
        : movement_speed_(movement_speed), mouse_sensitivity_(mouse_sensitivity), enable_physics_(enable_physics) {
    }

    void on_init() override {
        std::cout << "PlayerController initialized for entity: " << get_owner()->get_name() << std::endl;

        // Initialize variables
        set_float("health", 100.0f);
        set_bool("is_grounded", true);
        set_bool("can_jump", true);

        // Initialize camera orientation
        yaw_ = -90.0f; // Start looking forward (negative Z)
        pitch_ = 0.0f; // Start looking straight ahead

        // Setup camera if it exists (no movement speed - that's our responsibility)
        auto *camera = get_component<DCraft::CameraComponent>();
        if (camera) {
            std::cout << "Camera configured for player" << std::endl;
        }

        // Initialize physics variables if physics is enabled
        if (enable_physics_) {
            velocity_ = glm::vec3(0.0f);
            gravity_ = -9.81f;
            jump_force_ = 8.0f;
            friction_ = 0.8f;
        }
    }

    void on_update(float delta_time) override {
        // Handle keyboard input every frame
        handle_keyboard_input(delta_time);

        // Update physics if enabled
        if (enable_physics_) {
            update_physics(delta_time);
        }

        // Check if player fell off the world
        auto *transform = get_transform();
        if (transform && transform->get_position().y < -50.0f) {
            respawn();
        }
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

    void set_physics_enabled(bool enabled) { enable_physics_ = enabled; }
    bool is_physics_enabled() const { return enable_physics_; }

    // Camera orientation getters
    float get_yaw() const { return yaw_; }
    float get_pitch() const { return pitch_; }

    // Public methods for external events
    void take_damage(float damage) {
        float current_health = get_float("health", 100.0f);
        current_health -= damage;
        set_float("health", std::max(0.0f, current_health));

        std::cout << "Player took " << damage << " damage. Health: " << current_health << std::endl;

        if (current_health <= 0.0f) {
            die();
        }
    }

    void heal(float amount) {
        float current_health = get_float("health", 100.0f);
        current_health += amount;
        set_float("health", std::min(100.0f, current_health));

        std::cout << "Player healed for " << amount << ". Health: " << current_health << std::endl;
    }

private:
    float movement_speed_;
    float mouse_sensitivity_;
    bool enable_physics_;

    // Camera orientation (controlled by PlayerController)
    float yaw_ = -90.0f;
    float pitch_ = 0.0f;

    // Physics variables (only used if enable_physics_ is true)
    glm::vec3 velocity_ = glm::vec3(0.0f);
    float gravity_ = -9.81f;
    float jump_force_ = 8.0f;
    float friction_ = 0.8f;

    void update_camera_orientation() {
        auto *camera = get_component<DCraft::CameraComponent>();
        if (!camera) return;

        // Calculate camera direction vectors from yaw and pitch
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front = glm::normalize(front);

        // Calculate right and up vectors
        glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(front, world_up));
        glm::vec3 up = glm::normalize(glm::cross(right, front));

        // Update camera's internal vectors by setting yaw/pitch directly
        // (This assumes your CameraComponent has these setters)
        camera->set_yaw(yaw_);
        camera->set_pitch(pitch_);
    }

    void handle_keyboard_input(float delta_time) {
        auto &app = DCraft::Application::get_instance();

        // Handle movement
        if (enable_physics_) {
            handle_physics_movement(delta_time);
        } else {
            handle_direct_movement(delta_time);
        }

        // Handle special actions
        if (app.is_key_pressed(' ')) {
            // Space for jump
            jump();
        }

        // Handle other actions
        if (app.is_key_pressed('f') || app.is_key_pressed('F')) {
            on_interact();
        }
    }

    void handle_direct_movement(float delta_time) {
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
            // Left Shift for sprint
            current_speed *= 2.0f;
        }

        // Normalize movement direction to prevent faster diagonal movement
        if (glm::length(movement_direction) > 0.0f) {
            movement_direction = glm::normalize(movement_direction);
        }

        // Apply movement to transform
        DCraft::TransformComponent* transform = get_transform();
        if (transform) {
            glm::vec3 current_position = transform->get_position();
            glm::vec3 new_position = current_position + movement_direction * current_speed * delta_time;
            transform->set_position(new_position.x, new_position.y, new_position.z);
            camera->invalidate_view();
        }
        
    }

    void handle_physics_movement(float delta_time) {
        auto &app = DCraft::Application::get_instance();
        auto *camera = get_component<DCraft::CameraComponent>();
        if (!camera) return;

        glm::vec3 input_direction(0.0f);

        // Get input direction
        if (app.is_key_pressed('w') || app.is_key_pressed('W')) {
            input_direction += camera->get_front_vector();
        }
        if (app.is_key_pressed('s') || app.is_key_pressed('S')) {
            input_direction -= camera->get_front_vector();
        }
        if (app.is_key_pressed('a') || app.is_key_pressed('A')) {
            input_direction -= camera->get_right_vector();
        }
        if (app.is_key_pressed('d') || app.is_key_pressed('D')) {
            input_direction += camera->get_right_vector();
        }

        // Flatten input direction (no Y movement from camera angle for ground movement)
        input_direction.y = 0.0f;
        if (glm::length(input_direction) > 0.0f) {
            input_direction = glm::normalize(input_direction);
        }

        // Apply acceleration
        float acceleration = movement_speed_ * 10.0f;
        if (app.is_key_pressed(16)) {
            // Sprint
            acceleration *= 2.0f;
        }

        // Apply horizontal forces
        velocity_ += input_direction * acceleration * delta_time;

        // Apply friction to horizontal movement
        velocity_.x *= friction_;
        velocity_.z *= friction_;
    }

    void update_physics(float delta_time) {
        auto *transform = get_transform();
        if (!transform) return;

        // Apply gravity
        if (!get_bool("is_grounded", true)) {
            velocity_.y += gravity_ * delta_time;
        }

        // Apply velocity to position
        glm::vec3 position = transform->get_position();
        position += velocity_ * delta_time;

        // Simple ground check (you'd want proper collision detection)
        if (position.y <= 0.0f) {
            position.y = 0.0f;
            velocity_.y = 0.0f;
            set_bool("is_grounded", true);
            set_bool("can_jump", true);
        } else {
            set_bool("is_grounded", false);
        }

        transform->set_position(position.x, position.y, position.z);
    }

    void jump() {
        if (enable_physics_ && get_bool("can_jump", true) && get_bool("is_grounded", true)) {
            velocity_.y = jump_force_;
            set_bool("is_grounded", false);
            set_bool("can_jump", false);
            std::cout << "Player jumped!" << std::endl;
        }
    }

    void on_interact() {
        std::cout << "Player interacted!" << std::endl;
        // Add interaction logic here - could raycast for objects, open doors, etc.
    }

    void die() {
        std::cout << "Player died!" << std::endl;
        set_bool("is_dead", true);
        // Could trigger death animation, game over screen, etc.
        respawn(); // Auto-respawn for now
    }

    void respawn() {
        std::cout << "Player respawning..." << std::endl;
        auto *transform = get_transform();
        if (transform) {
            transform->set_position(0, 2, 0); // Spawn position
        }
        set_float("health", 100.0f);
        set_bool("is_dead", false);
        if (enable_physics_) {
            velocity_ = glm::vec3(0.0f);
        }

        // Reset camera orientation
        yaw_ = -90.0f;
        pitch_ = 0.0f;
        update_camera_orientation();
    }
};
