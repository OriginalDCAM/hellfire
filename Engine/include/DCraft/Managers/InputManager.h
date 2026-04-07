//
// Created by denzel on 18/09/2025.
//

#pragma once
#include <algorithm>

#include "DCraft/IWindow.h"
#include "DCraft/Utility/ServiceLocator.h"
#include "glm/detail/type_vec.hpp"

namespace hellfire {
    class InputManager {
    private:
        bool keys_[512] = {false};
        bool keys_just_pressed_[512] = {false};
        bool keys_just_released_[512] = {false};
        glm::vec2 mouse_position_{0.0f, 0.0f};
        bool mouse_buttons_[8] = {false};

    public:
        // Public interface for scripts
        bool is_key_pressed(int keycode) const { return keys_[keycode]; }
        bool is_key_just_pressed(int keycode) const { return keys_just_pressed_[keycode]; }
        bool is_mouse_button_pressed(int button) const { return mouse_buttons_[button]; }
        glm::vec2 get_mouse_position() const { return mouse_position_; }

        void set_cursor_mode(const CursorMode mode) {
            auto *window = ServiceLocator::get_service<IWindow>();
            if (window) {
                window->set_cursor_mode(mode);
            }
        }

        // Called by Application (via event handlers)
        void on_key_down(int key) {
            if (key >= 0 && key < 512) {
                if (!keys_[key]) keys_just_pressed_[key] = true;
                keys_[key] = true;
            }
        }

        void on_key_up(int key) {
            if (key >= 0 && key < 512) {
                if (keys_[key]) keys_just_released_[key] = true;
                keys_[key] = false;
            }
        }

        void on_mouse_button(int button, bool pressed) {
            if (button >= 0 && button < 8) {
                mouse_buttons_[button] = pressed;
            }
        }

        void on_mouse_move(float x, float y) {
            mouse_position_ = {x, y};
        }

        void update() {
            // Clear frame flags
            std::fill_n(keys_just_pressed_, 512, false);
            std::fill_n(keys_just_released_, 512, false);
        }
    };
}
