//
// Created by denzel on 14/08/2025.
//

#include "UI/Panels/Viewport/SceneCameraScript.h"

#include "hellfire/core/Application.h"
#include "hellfire/ecs/CameraComponent.h"
#include "hellfire/utilities/ServiceLocator.h"
#include "GLFW/glfw3.h"
#include "hellfire/scene/Scene.h"

SceneCameraScript::SceneCameraScript (float movement_speed, float mouse_sensitivity) 
    : movement_speed_(movement_speed), mouse_sensitivity_(mouse_sensitivity) {
}

void SceneCameraScript::on_init() {
    std::cout << "SceneCameraScript initialized for entity: " << get_owner().get_name() << std::endl;
    REGISTER_VAR(look_speed, FLOAT);

    // Initialize camera orientation
    yaw_ = -80.0f;
    pitch_ = 0.0f;
}

void SceneCameraScript::on_update(float delta_time) {
    handle_keyboard_look(delta_time);
    handle_movement(delta_time);
}

void SceneCameraScript::handle_keyboard_look(float delta_time) {
    auto input_manager = hellfire::ServiceLocator::get_service<hellfire::InputManager>();
    
    if (input_manager->is_key_pressed(GLFW_KEY_J)) { // Look left
        yaw_ -= look_speed * delta_time;
    }
    if (input_manager->is_key_pressed(GLFW_KEY_L)) { // Look right
        yaw_ += look_speed * delta_time;
    }
    if (input_manager->is_key_pressed(GLFW_KEY_I)) { // Look up
        pitch_ += look_speed * delta_time;
    }
    if (input_manager->is_key_pressed(GLFW_KEY_K)) { // Look down
        pitch_ -= look_speed * delta_time;
    }
    
    // Constrain pitch
    pitch_ = glm::clamp(pitch_, -89.0f, 89.0f);
    
    // Update camera
    update_camera_orientation();
}

void SceneCameraScript::handle_movement(float delta_time) const {
    auto input_manager = hellfire::ServiceLocator::get_service<hellfire::InputManager>();
    auto *camera = get_component<hellfire::CameraComponent>();
    if (!camera) return;

    glm::vec3 movement_direction(0.0f);

    // Full 3D movement 
    if (input_manager->is_key_pressed(GLFW_KEY_W)) {
        movement_direction += camera->get_front_vector(); 
    }
    if (input_manager->is_key_pressed(GLFW_KEY_S)) {
        movement_direction -= camera->get_front_vector(); 
    }
    if (input_manager->is_key_pressed(GLFW_KEY_A)) {
        movement_direction -= camera->get_right_vector();
    }
    if (input_manager->is_key_pressed(GLFW_KEY_D)) {
        movement_direction += camera->get_right_vector();
    }
    if (input_manager->is_key_pressed(GLFW_KEY_Q)) {
        movement_direction -= camera->get_up_vector(); 
    }
    if (input_manager->is_key_pressed(GLFW_KEY_E)) {
        movement_direction += camera->get_up_vector();
    }

    float current_speed = movement_speed_;

    // Normalize movement direction
    if (glm::length(movement_direction) > 0.0f) {
        movement_direction = glm::normalize(movement_direction);
    }

    // Apply movement to transform
    if (hellfire::TransformComponent *transform = get_transform()) {
        glm::vec3 current_position = transform->get_position();
        glm::vec3 new_position = current_position + movement_direction * current_speed * delta_time;
        transform->set_position(new_position.x, new_position.y, new_position.z);
        camera->invalidate_view();
    }
}

void SceneCameraScript::on_remove() {
    std::cout << "SceneCameraScript removed from entity: " << get_owner().get_name() << std::endl;
}

void SceneCameraScript::handle_mouse_movement(float x_offset, float y_offset) {
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

void SceneCameraScript::update_camera_orientation() const {
    auto *camera = get_component<hellfire::CameraComponent>();
    if (!camera) return;

    // Update camera's internal vectors by setting yaw/pitch directly
    camera->set_yaw(yaw_);
    camera->set_pitch(pitch_);
}


