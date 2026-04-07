//
// Created by denzel on 17/08/2025.
//

#include "Scripts/PlayerController.h"

#include "DCraft/Application.h"
#include "DCraft/Components/CameraComponent.h"

PlayerController::PlayerController(float movement_speed, float mouse_sensitivity) : movement_speed_(movement_speed),
    mouse_sensitivity_(mouse_sensitivity) {
}

void PlayerController::on_init() {
    std::cout << "PlayerController initialized for entity: " << get_owner()->get_name() << std::endl;

    // Initialize camera orientation
    yaw_ = -90.0f;
    pitch_ = 0.0f;
}

void PlayerController::on_update(float delta_time) {
    handle_movement(delta_time);
}

void PlayerController::on_remove() {
    std::cout << "PlayerController removed from entity: " << get_owner()->get_name() << std::endl;
}

void PlayerController::handle_mouse_movement(float x_offset, float y_offset) {
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

void PlayerController::update_camera_orientation() const {
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

void PlayerController::handle_movement(float delta_time) const {
    auto &app = DCraft::Application::get_instance();
    auto *camera = get_component<DCraft::CameraComponent>();
    if (!camera) return;

    // Calculate movement direction based on camera orientation
    glm::vec3 movement_direction(0.0f);

    if (app.is_key_pressed('w')) {
        movement_direction += glm::vec3(camera->get_front_vector().x, 0, camera->get_front_vector().z);
    }
    if (app.is_key_pressed('s')) {
        movement_direction -= glm::vec3(camera->get_front_vector().x, 0, camera->get_front_vector().z);
    }
    if (app.is_key_pressed('a')) {
        movement_direction -= camera->get_right_vector();
    }
    if (app.is_key_pressed('d')) {
        movement_direction += camera->get_right_vector();
    }
    if (app.is_key_pressed('e')) {
        movement_direction -= glm::vec3(0, camera->get_up_vector().y, 0);
    }
    if (app.is_key_pressed('q')) {
        movement_direction += glm::vec3(0, camera->get_up_vector().y, 0);
    }

    // Apply movement speed and sprint modifier
    float current_speed = movement_speed_;
    if (app.is_shift_pressed()) {
        current_speed *= 2.0f; // Sprint speed
    }

    // Normalize movement direction to prevent faster diagonal movement
    if (glm::length(movement_direction) > 0.0f) {
        movement_direction = glm::normalize(movement_direction);
    }

    // Apply movement to transform
    if (DCraft::TransformComponent *transform = get_transform()) {
        glm::vec3 current_position = transform->get_position();
        glm::vec3 new_position = current_position + movement_direction * current_speed * delta_time;
        transform->set_position(new_position.x, new_position.y, new_position.z);
        camera->invalidate_view();
    }
}

void PlayerController::on_interact() {
    // TODO: raycast from cursor to object for object information (planet name, orbit speed, avg temperature, etc.)
    std::cout << "Player interacted!" << std::endl;
}
