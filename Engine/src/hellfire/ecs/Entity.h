//
// Created by denzel on 07/08/2025.
//
#pragma once

#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>


namespace hellfire {
    class TransformComponent;
    class Component;
    class ScriptComponent;
}

namespace hellfire {
    class Entity {
    public:
        virtual ~Entity() {
            cleanup_scripts();

            for (const auto *child: children_) {
                std::clog << "Deleting child named: " << child->get_name() << ", from the parent: " << get_name() <<
                        '\n';
                delete child;
            }
        }

        explicit Entity(const std::string &name);


        // Add an entity component
        void add(Entity *child);

        void remove(Entity *child);

        // Getters
        [[nodiscard]] const std::string &get_name() const { return name_; }
        [[nodiscard]] const std::vector<Entity *> &get_children() const { return children_; }
        [[nodiscard]] Entity *get_parent() const { return parent_; }

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

        [[nodiscard]] const std::vector<ScriptComponent *> &get_script_components() const;

        void setup_recursive() {
            if (!initialized_) {
                setup();
            }
        }

        // Recursive cleanup method
        void cleanup_recursive() {
            // Cleanup children first
            for (auto *child: children_) {
                child->cleanup_recursive();
            }

            // Then cleanup self
            cleanup_scripts();
            initialized_ = false;
        }

        void setup_children_recursive() const {
            for (auto *child: children_) {
                child->setup_recursive();
            }
        }

        void update_children_recursive(const float delta_time) const {
            for (auto *child: children_) {
                child->update(delta_time);
            }
        }

        // Lifecycle management
        virtual void setup() {
            initialize_scripts();
            initialized_ = true;

            setup_children_recursive();
        }

        virtual void update(const float delta_time) {
            update_scripts(delta_time);

            update_children_recursive(delta_time);
        }

        void update_world_matrices() const;

        // Script specific methods
        void initialize_scripts() const;

        void update_scripts(const float delta_time) const;

        void cleanup_scripts() const;

        // Event broadcasting to all script components
        void broadcast_event(const std::string &event_name, void *data = nullptr, const bool recursive = false) const;

        // Send event to a specific script component type
        template<typename T>
        void send_event_to_script(const std::string &event_name, void *data = nullptr, const bool recursive = false);

        // Setters
        void set_name(const std::string &name) { name_ = name; }

        // Convenience methods for transform
        TransformComponent *transform();

        [[nodiscard]] const TransformComponent *transform() const;

    private:
        std::unordered_map<std::type_index, std::unique_ptr<Component> > components_;
        std::vector<Entity *> children_;
        std::vector<ScriptComponent *> script_components_;
        Entity *parent_ = nullptr;
        std::string name_;
        bool initialized_ = false;
    };
    
#include "Entity.inl"
} // namespace hellfire

