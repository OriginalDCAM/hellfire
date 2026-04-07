//
// Created by denzel on 11/08/2025.
//
#include "Scripts/OrbitController.h"
#include <iostream>
#include <cmath>
#include "hellfire/ecs/TransformComponent.h"
#include "../../../engine/src/hellfire/ecs/Entity.h"
#include "Utils/AnimationInputHandler.h"

void OrbitController::on_init() {
    current_angle_ = 0.0f;
    current_rotation_ = 0.0f;

    if (const auto *transform = get_transform()) {
        initial_position_ = transform->get_position() - center_;
        current_angle_ = atan2(initial_position_.z, initial_position_.x);
    }

    AnimationInputHandler::get_instance().register_orbit_controller(this);

    std::cout << "OrbitController initialized for: " << get_owner().get_name() << std::endl;
 
}

void OrbitController::on_update(float delta_time) {
    if (!is_running_) return;

    auto *transform = get_transform();
    if (!transform) return;

    // Apply speed multiplier
    float modified_delta = delta_time * speed_multiplier_;

    // Update orbital angle
    current_angle_ += orbit_speed_ * modified_delta;

    // Calculate new orbital position
    glm::vec3 orbital_position;
    orbital_position.x = center_.x + cos(current_angle_) * orbit_radius_;
    orbital_position.y = center_.y;
    orbital_position.z = center_.z + sin(current_angle_) * orbit_radius_;

    // Apply orbital position
    transform->set_position(orbital_position);

    // Update planet rotation 
    current_rotation_ += rotation_speed_ * modified_delta;
    transform->set_rotation(0.0f, glm::degrees(current_rotation_), 0.0f);
}

OrbitController::~OrbitController() {
    AnimationInputHandler::get_instance().unregister_orbit_controller(this);
}
