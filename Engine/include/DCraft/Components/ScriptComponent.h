//
// Created by denzel on 10/08/2025.
//

#pragma once
#include <string>
#include <unordered_map>

#include "DCraft/Structs/Entity.h"
#include "DCraft/Structs/Component.h"

namespace hellfire {
    class TransformComponent;

    class ScriptComponent : public Component {
    public:
        ScriptComponent() = default;

        virtual ~ScriptComponent() = default;

        // Virtual lifecycle methods
        virtual void on_init() {
        };

        virtual void on_update(float delta_time) {
        }

        virtual void on_remove() {
        }

        virtual void on_event(const std::string &event_name, void *data = nullptr) {
        }

        // Called by the entity system
        void init() {
            on_init();
        }

        void update(float delta_time) {
            on_update(delta_time);
        }

        void remove() {
            on_remove();
        }

        void trigger_event(const std::string &event_name, void *data = nullptr) {
            on_event(event_name, data);
        }

        // Variable management for script state
        void set_float(const std::string &name, float value) { float_vars_[name] = value; }

        float get_float(const std::string &name, float default_val = 0.0f) const {
            auto it = float_vars_.find(name);
            return it != float_vars_.end() ? it->second : default_val;
        }

        void set_bool(const std::string &name, bool value) { bool_vars_[name] = value; }

        bool get_bool(const std::string &name, bool default_val = false) const {
            auto it = bool_vars_.find(name);
            return it != bool_vars_.end() ? it->second : default_val;
        }

        void set_string(const std::string &name, const std::string &value) { string_vars_[name] = value; }

        std::string get_string(const std::string &name, const std::string &default_val = "") const {
            auto it = string_vars_.find(name);
            return it != string_vars_.end() ? it->second : default_val;
        }

        // Utility methods for common operations
        TransformComponent *get_transform() const;

        // Helper method to check if the owner has a specific component
        template<typename T>
        bool has_component() const {
            return get_owner() ? get_owner()->template has_component<T>() : false;
        }

        template<typename T>
        T *get_component() const {
            return get_owner() ? get_owner()->template get_component<T>() : nullptr;
        }

    protected:
        // Protected variables that derived classes can access
        std::unordered_map<std::string, float> float_vars_;
        std::unordered_map<std::string, bool> bool_vars_;
        std::unordered_map<std::string, std::string> string_vars_;
    };
}
