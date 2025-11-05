//
// Created by denzel on 07/08/2025.
//
#pragma once

#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Component.h"


namespace hellfire {
    class TransformComponent;
    class ScriptComponent;
}

class Test {
    bool is_looking = true;
};

namespace hellfire {
    using EntityID = uint32_t;
    
    class Entity {
    public:
        virtual ~Entity() = default;

        explicit Entity(EntityID id, const std::string &name)
            : id_(id), name_(name) {
        }

        // Identification
        [[nodiscard]] uint32_t get_id() const { return id_; }
        [[nodiscard]] const std::string &get_name() const { return name_; }
        void set_name(const std::string &name) { name_ = name; }

        // Component management
        template<typename T, typename... Args>
        T *add_component(Args &&... args);

        template<typename T>
        [[nodiscard]] T *get_component() const;

        template<typename T>
        bool has_component() {
            return components_.find(std::type_index(typeid(T))) != components_.end();
        }

        template<typename T>
        bool remove_component();

        // Script management
        [[nodiscard]] const std::vector<ScriptComponent *> &get_script_components() const;

        void initialize_scripts() const;

        void update_scripts(float delta_time) const;

        void cleanup_scripts() const;

        // Event system (non-recursive)
        void broadcast_event(const std::string &event_name, void *data = nullptr) const;

        template<typename T>
        void send_event_to_script(const std::string &event_name, void *data = nullptr);

        // Convenience methods
        TransformComponent *transform();

        [[nodiscard]] const TransformComponent *transform() const;

    private:
        EntityID id_;
        std::string name_;
        std::unordered_map<std::type_index, std::unique_ptr<Component> > components_;
        std::vector<ScriptComponent *> script_components_;
    };

#include "Entity.inl"
} // namespace hellfire
