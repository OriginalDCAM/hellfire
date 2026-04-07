//
// Created by denzel on 11/08/2025.
//
#include "Scripts/OrbitController.h"

#include <iostream>

#include "DCraft/Components/TransformComponent.h"
#include "DCraft/Structs/Entity.h"

void OrbitController::on_init() {
    // Initialize orbital parameters
    current_angle_ = 0.0f;
    current_rotation_ = 0.0f;

    auto* transform = get_transform();
    if (transform) {
        initial_position_ = transform->get_position() - center_;
        // Calculate an initial angle based on position
        current_angle_ = atan2(initial_position_.z, initial_position_.x);
    }

    std::cout << "OrbitController initialized for: " << get_owner()->get_name() << std::endl;
}

void OrbitController::on_update(float delta_time) {
    auto* transform = get_transform();
    if (!transform) return;

    // Update orbital angle
    current_angle_ += orbit_speed_ * delta_time;

    // Calculate new orbital position
    glm::vec3 orbital_position;
    orbital_position.x = center_.x + cos(current_angle_) * orbit_radius_;
    orbital_position.y = center_.y;
    orbital_position.z = center_.z + sin(current_angle_) * orbit_radius_;

    // Apply orbital position
    transform->set_position(orbital_position);

    // Update planet rotation (spinning on its own axis)
    current_rotation_ += rotation_speed_ * delta_time;
    transform->set_rotation(0.0f, glm::degrees(current_rotation_), 0.0f);
}
