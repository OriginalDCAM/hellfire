//
// Created by denzel on 15/08/2025.
//

#pragma once
#include <vector>
#include "Scripts/OrbitController.h"

class AnimationInputHandler {
public:
    static AnimationInputHandler& get_instance() {
        static AnimationInputHandler instance;
        return instance;
    }

    void register_orbit_controller(OrbitController* controller);
    void unregister_orbit_controller(OrbitController* controller);
    void handle_input(); 
private:
    std::vector<OrbitController*> orbit_controllers_;
    bool space_was_pressed_last_frame_ = false;
    bool key_1_was_pressed_last_frame_ = false;
    bool key_2_was_pressed_last_frame_ = false;
    bool key_3_was_pressed_last_frame_ = false;
    bool key_4_was_pressed_last_frame_ = false;
    
    void toggle_all_animations() const;
    void set_all_speed_multiplier(float multiplier);
};