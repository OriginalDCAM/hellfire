//
// Created by denzel on 11/08/2025.
//
#pragma once
#include <glm/detail/type_vec.hpp>
#include <glm/detail/type_vec3.hpp>

#include "DCraft/Components/ScriptComponent.h"

class OrbitController : public DCraft::ScriptComponent {
public:
    OrbitController(float orbit_radius = 10.0f, float orbit_speed = 2.0f, float rotation_speed = 2.0f) 
        : orbit_radius_(orbit_radius), orbit_speed_(orbit_speed), rotation_speed_(rotation_speed) {}

    void on_init() override;
    void on_update(float delta_time) override;

    ~OrbitController();

    // Animation control
    void toggle_animation() { is_running_ = !is_running_; }
    bool is_animating() const { return is_running_; }
    
    void set_speed_multiplier(float multiplier) { 
        speed_multiplier_ = multiplier; 
    }
    float get_speed_multiplier() const { return speed_multiplier_; }


    // Setters
    void set_orbit_radius(float radius) { orbit_radius_ = radius; }
    void set_orbit_speed(float speed) { orbit_speed_ = speed; }
    void set_rotation_speed(float speed) { rotation_speed_ = speed; }
    void set_center(const glm::vec3& center) { center_ = center; }

    // Getters
    float get_orbit_radius() const { return orbit_radius_; }
    float get_orbit_speed() const { return orbit_speed_; }
    float get_rotation_speed() const { return rotation_speed_; }
    glm::vec3 get_center() const { return center_; }
    float get_current_angle() const { return current_angle_; }

private:
    // Orbital parameters
    float orbit_radius_;
    float orbit_speed_;
    float rotation_speed_;
    float current_angle_ = 0.0f;
    float current_rotation_ = 0.0f;

    // State
    bool is_running_ = true;
    float speed_multiplier_ = 1.0f;
    
    // Transform data
    glm::vec3 center_ = glm::vec3(0.0f);
    glm::vec3 initial_position_;
};

