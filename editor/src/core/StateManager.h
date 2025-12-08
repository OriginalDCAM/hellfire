//
// Created by denzel on 05/12/2025.
//

#pragma once
#include <memory>
#include <ranges>
#include <typeindex>
#include <unordered_map>

#include "ApplicationState.h"

namespace hellfire::editor {
    class StateManager {
    public:
        template<typename  T, typename... Args>
        void register_state(Args&&... args) {
            auto state = std::make_unique<T>(std::forward<Args>(args)...);
            states_[std::type_index(typeid(T))] = std::move(state);
        }

        template<typename T>
        void switch_to() {
            auto it = states_.find(std::type_index(typeid(T)));
            if (it == states_.end()) return;

            if (current_state_) {
                current_state_->on_exit();
            }
            current_state_ = it->second.get();
            current_state_->on_enter();
        }

        void set_context(EditorContext* ctx) {
            for (const auto &state: states_ | std::views::values) {
                state->set_context(ctx);
            }
        }

        void render() const {
            if (current_state_) {
                current_state_->render();
            }
        }

        bool on_mouse_move(const float x, const float y, const float x_offset, const float y_offset) const {
            return current_state_ ? current_state_->on_mouse_move(x, y, x_offset, y_offset) : false;
        }
    
        bool on_mouse_button(const int button, const bool pressed) const {
            return current_state_ ? current_state_->on_mouse_button(button, pressed) : false;
        }
    
        bool on_mouse_wheel(const float delta) const {
            return current_state_ ? current_state_->on_mouse_wheel(delta) : false;
        }
    
        bool on_key_down(const int key) const {
            return current_state_ ? current_state_->on_key_down(key) : false;
        }
    
        bool on_key_up(const int key) const {
            return current_state_ ? current_state_->on_key_up(key) : false;
        }
    
        Entity* get_render_camera_override() const {
            return current_state_ ? current_state_->get_render_camera_override() : nullptr;
        }

    private:
        std::unordered_map<std::type_index, std::unique_ptr<ApplicationState>> states_;
        ApplicationState* current_state_ = nullptr;
    };
}
