//
// Created by denzel on 07/08/2025.
//
#pragma once

#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "DCraft/Components/ScriptComponent.h"
#include "DCraft/Components/TransformComponent.h"

namespace DCraft {
    class Component;
}

namespace DCraft {
    class Entity {
    public:
        virtual ~Entity() {
            cleanup_scripts();
            
            for (auto *child: children_) {
                std::clog << "Deleting child named: " << child->get_name() << ", from the parent: " << get_name() <<
                        '\n';
                delete child;
            }
        }

        explicit Entity(const std::string &name) {
            set_name(name);
            add_component<TransformComponent>();
        }

        // Add an entity component
        void add(Entity *child) {
            auto entity = std::find(children_.begin(), children_.end(), child);
            if (entity == children_.end()) {
                if (child->parent_ != nullptr) {
                    child->parent_->remove(child);
                }

                children_.push_back(child);
                child->parent_ = this;
            }
        }

        void remove(Entity *child) {
            const auto entity = std::find(children_.begin(), children_.end(), child);
            if (entity != children_.end()) {
                // Remove from children's list
                children_.erase(entity);
                child->parent_ = nullptr;
            }
        }

        // Getters
        const std::string &get_name() const { return name_; }
        const std::vector<Entity *> &get_children() const { return children_; }
        Entity *get_parent() const { return parent_; }

        // Component management
        template<typename T, typename... Args>
        T *add_component(Args &&... args) {
            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            T *component_ptr = component.get();
            components_[std::type_index(typeid(T))] = std::move(component);

            // Call lifecycle hook if Component base class has it
            if constexpr (std::is_base_of_v<Component, T>) {
                component_ptr->on_added(this);
            }

            if constexpr (std::is_base_of_v<ScriptComponent, T>) {
                script_components_.push_back(component_ptr);
                if (initialized_) {
                    component_ptr->init(); // Initialize immediately if entity is already initialized
                }
            }

            return component_ptr;
        }

        template<typename T>
        T *get_component() const {
            auto it = components_.find(std::type_index(typeid(T)));
            if (it != components_.end()) {
                return static_cast<T *>(it->second.get());
            }
            return nullptr;
        }

        template<typename T>
        bool has_component() {
            return components_.find(std::type_index(typeid(T))) != components_.end();
        }

        template<typename T>
        bool remove_component() {
            auto it = components_.find(std::type_index(typeid(T)));
            if (it != components_.end()) {
                T* component_ptr = static_cast<T*>(it->second.get());

                // Special handling for ScriptComponents
                if constexpr (std::is_base_of_v<ScriptComponent, T>) {
                    component_ptr->remove(); // Call script cleanup
                    // Remove from script components list
                    auto script_it = std::find(script_components_.begin(), script_components_.end(), component_ptr);
                    if (script_it != script_components_.end()) {
                        script_components_.erase(script_it);
                    }
                }
                
                if constexpr (std::is_base_of_v<Component, T>) {
                    component_ptr->on_removed();
                }
                components_.erase(it);
                return true;
            }
            return false;
        }

        const std::vector<ScriptComponent*>& get_script_components() const {
            return script_components_;
        }

        // Lifecycle management
        virtual void setup() {
            initialize_scripts();
            initialized_ = true;
        }

        virtual void update(float delta_time) {
            update_scripts(delta_time);
        }

        void update_world_matrices() {
            auto *transform = get_component<TransformComponent>();
            if (transform) {
                const glm::mat4 *parent_world = nullptr;
                if (parent_ && parent_->has_component<TransformComponent>()) {
                    parent_world = &parent_->get_component<TransformComponent>()->get_world_matrix();
                }
                transform-> update_world_matrix(parent_world);
            }

            // Update children
            for (auto *child: children_) {
                child->update_world_matrices();
            }
        }

        // Script specific methods
        void initialize_scripts() {
            for (auto* script : script_components_) {
                script->init();
            }
        }

        void update_scripts(float delta_time) {
            for (auto* script : script_components_) {
                script->update(delta_time);
            }
        }

        void cleanup_scripts() {
            for (auto* script : script_components_) {
                script->remove();
            }
        }

        // Event broadcasting to all script components
        void broadcast_event(const std::string& event_name, void* data = nullptr) {
            for (auto* script : script_components_) {
                script->trigger_event(event_name, data);
            }
        }

        // Send event to a specific script component type
        template<typename T>
        void send_event_to_script(const std::string& event_name, void* data = nullptr) {
            static_assert(std::is_base_of_v<ScriptComponent, T>, "T must derive from ScriptComponent");
            auto* script = get_component<T>();
            if (script) {
                script->trigger_event(event_name, data);
            }
        }

        // Setters
        void set_name(const std::string &name) { name_ = name; }

        // Convenience methods for transform
        TransformComponent *transform() {
            auto *comp = get_component<TransformComponent>();
            if (!comp) {
                std::clog << "Warning: Entity '" << get_name()
                        << "' missing TransformComponent, auto-creating\n";
                comp = add_component<TransformComponent>();
            }
            return comp;
        }

        const TransformComponent *transform() const { return get_component<TransformComponent>(); }

    private:
        std::unordered_map<std::type_index, std::unique_ptr<Component> > components_;
        std::vector<Entity *> children_;
        std::vector<ScriptComponent*> script_components_; // Quick access to script components
        Entity *parent_ = nullptr;
        std::string name_;
        bool initialized_ = false; // Track if entity has been initialized
    };
} // namespace DCraft
