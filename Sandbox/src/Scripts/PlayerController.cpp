//
// Created by denzel on 14/08/2025.
//

#include "Scripts/PlayerController.h"

#include "DCraft/Application.h"
#include "DCraft/Components/CameraComponent.h"

PlayerController::PlayerController(float movement_speed, float mouse_sensitivity) 
    : movement_speed_(movement_speed), mouse_sensitivity_(mouse_sensitivity) {
}

void PlayerController::on_init() {
    std::cout << "PlayerController initialized for entity: " << get_owner()->get_name() << std::endl;

    last_known_player_position = get_transform()->get_position();

    // Initialize camera orientation
    yaw_ = -90.0f;
    pitch_ = 0.0f;
}

void PlayerController::on_update(float delta_time) {
    handle_drone_mode(delta_time);
    handle_movement(delta_time);
}

void PlayerController::handle_drone_mode(float delta_time) {
    static float time_since_last_v_press = 0.2f; 
    
    time_since_last_v_press += delta_time;
    
    if (DCraft::Application::get_instance().is_key_pressed('v') && time_since_last_v_press > 1.0f) {
        time_since_last_v_press = 0.0f;
        
        if (drone_mode_) {
            // Switching FROM drone mode TO free flight
            drone_mode_ = false;
            restore_free_flight_position();
        } else {
            // Switching FROM free flight TO drone mode
            save_free_flight_position();
            drone_mode_ = true;
            set_drone_overview_position();
        }
    }
    
    // Handle keyboard looking
    handle_keyboard_look(delta_time);
}

void PlayerController::save_free_flight_position() {
    const auto* transform = get_transform();
    if (!transform) return;
    
    // Save the current position and orientation
    last_known_player_position = transform->get_position();
    saved_yaw_ = yaw_;
    saved_pitch_ = pitch_;
}

void PlayerController::restore_free_flight_position() {
    auto* transform = get_transform();
    auto* camera = get_component<DCraft::CameraComponent>();
    if (!transform || !camera) return;
    
    transform->set_position(last_known_player_position);
    yaw_ = saved_yaw_;
    pitch_ = saved_pitch_;
    update_camera_orientation();
}

void PlayerController::set_drone_overview_position() {
    auto* transform = get_transform();
    auto* camera = get_component<DCraft::CameraComponent>();
    if (!transform || !camera) return;

    // Above Jupiter
    glm::vec3 overview_position(0.0f, 25.0f, 200.0f); 
    transform->set_position(overview_position);
    
    // Look at the center of the solar system (where the sun is)
    glm::vec3 look_at_target(0.0f, 0.0f, 0.0f);
    camera->look_at(look_at_target);
    
    yaw_ = camera->get_yaw();
    pitch_ = camera->get_pitch();
    
}

void PlayerController::handle_keyboard_look(float delta_time) {
    auto& app = DCraft::Application::get_instance();
    
    float look_speed = 45.0f; 
    
    if (app.is_key_pressed('j')) { // Look left
        yaw_ -= look_speed * delta_time;
    }
    if (app.is_key_pressed('l')) { // Look right
        yaw_ += look_speed * delta_time;
    }
    if (app.is_key_pressed('i')) { // Look up
        pitch_ += look_speed * delta_time;
    }
    if (app.is_key_pressed('k')) { // Look down
        pitch_ -= look_speed * delta_time;
    }
    
    // Constrain pitch
    pitch_ = glm::clamp(pitch_, -89.0f, 89.0f);
    
    // Update camera
    update_camera_orientation();
}

void PlayerController::handle_movement(float delta_time) const {
    auto &app = DCraft::Application::get_instance();
    auto *camera = get_component<DCraft::CameraComponent>();
    if (!camera) return;

    glm::vec3 movement_direction(0.0f);

    // Full 3D movement (perfect for space/drone mode)
    if (app.is_key_pressed('w')) {
        movement_direction += camera->get_front_vector(); 
    }
    if (app.is_key_pressed('s')) {
        movement_direction -= camera->get_front_vector(); 
    }
    if (app.is_key_pressed('a')) {
        movement_direction -= camera->get_right_vector();
    }
    if (app.is_key_pressed('d')) {
        movement_direction += camera->get_right_vector();
    }
    if (app.is_key_pressed('q')) {
        movement_direction -= camera->get_up_vector(); 
    }
    if (app.is_key_pressed('e')) {
        movement_direction += camera->get_up_vector();
    }

    float current_speed = movement_speed_;
    if (app.is_shift_pressed()) {
        current_speed *= 3.0f; 
    }
    if (app.is_ctrl_pressed()) {
        current_speed *= 0.3f; 
    }

    // Normalize movement direction
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

    // Update camera's internal vectors by setting yaw/pitch directly
    camera->set_yaw(yaw_);
    camera->set_pitch(pitch_);
}

void PlayerController::on_interact() {
    // TODO: raycast from cursor to object for object information (planet name, orbit speed, avg temperature, etc.)
    std::cout << "Player interacted!" << std::endl;
}
