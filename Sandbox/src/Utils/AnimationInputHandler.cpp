//
// Created by denzel on 17/08/2025.
//

#include "Utils/AnimationInputHandler.h"
#include "DCraft/Application.h"
#include <iostream>
#include <algorithm>

#include "DCraft/Utility/ServiceLocator.h"
#include "Utils/InputHandler.h"

void AnimationInputHandler::register_orbit_controller(OrbitController* controller) {
    orbit_controllers_.push_back(controller);
}

void AnimationInputHandler::unregister_orbit_controller(OrbitController* controller) {
    orbit_controllers_.erase(
        std::remove(orbit_controllers_.begin(), orbit_controllers_.end(), controller),
        orbit_controllers_.end()
    );
}

void AnimationInputHandler::handle_input() {
    auto input_manager = hellfire::ServiceLocator::get_service<hellfire::InputManager>();
    
    // Toggle all animations with spacebar
    bool space_pressed = input_manager->is_key_pressed(32); // Space
    if (space_pressed && !space_was_pressed_last_frame_) {
        toggle_all_animations();
    }
    space_was_pressed_last_frame_ = space_pressed;
    
    // Speed controls
    bool key_1_pressed = input_manager->is_key_pressed('1');
    if (key_1_pressed && !key_1_was_pressed_last_frame_) {
        set_all_speed_multiplier(0.5f);
    }
    key_1_was_pressed_last_frame_ = key_1_pressed;
    
    bool key_2_pressed = input_manager->is_key_pressed('2');
    if (key_2_pressed && !key_2_was_pressed_last_frame_) {
        set_all_speed_multiplier(1.0f);
    }
    key_2_was_pressed_last_frame_ = key_2_pressed;
    
    bool key_3_pressed = input_manager->is_key_pressed('3');
    if (key_3_pressed && !key_3_was_pressed_last_frame_) {
        set_all_speed_multiplier(2.0f);
    }
    key_3_was_pressed_last_frame_ = key_3_pressed;
    
    bool key_4_pressed = input_manager->is_key_pressed('4');
    if (key_4_pressed && !key_4_was_pressed_last_frame_) {
        set_all_speed_multiplier(5.0f);
    }
    key_4_was_pressed_last_frame_ = key_4_pressed;
}

void AnimationInputHandler::toggle_all_animations() const {
    for (auto* controller : orbit_controllers_) {
        if (controller) {
            controller->toggle_animation();
        }
    }
    std::cout << "Toggled all orbital animations" << std::endl;
}

void AnimationInputHandler::set_all_speed_multiplier(float multiplier) {
    for (auto* controller : orbit_controllers_) {
        if (controller) {
            controller->set_speed_multiplier(multiplier);
        }
    }
    std::cout << "Set orbit speed multiplier to: " << multiplier << "x" << std::endl;
}
