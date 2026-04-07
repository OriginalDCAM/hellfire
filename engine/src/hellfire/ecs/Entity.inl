//
// Created by denzel on 21/08/2025.
//

#pragma once

    template<ComponentType T, typename... Args>
    T *Entity::add_component(Args &&... args) {
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T *component_ptr = component.get();
        components_[std::type_index(typeid(T))] = std::move(component);

        // Call lifecycle hook if Component base class has it
        if constexpr (std::is_base_of_v<Component, T>) {
            component_ptr->on_added(this);
        }

        if constexpr (std::is_base_of_v<ScriptComponent, T>) {
            script_components_.push_back(component_ptr);
            component_ptr->init();
        }

        return component_ptr;
    }

    template<ComponentType T>
    T *Entity::get_component() const {
        if (const auto it = components_.find(std::type_index(typeid(T))); it != components_.end()) {
            return static_cast<T *>(it->second.get());
        }
        return nullptr;
    }

    template<ComponentType T>
    bool Entity::remove_component() {
        const auto it = components_.find(std::type_index(typeid(T)));
        if (it != components_.end()) {
            T *component_ptr = static_cast<T *>(it->second.get());

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

    template<ScriptComponentType T>
    void Entity::send_event_to_script(const std::string &event_name, void *data) {
        static_assert(std::is_base_of_v<ScriptComponent, T>, "T must derive from ScriptComponent");
        if (auto *script = get_component<T>()) {
            script->trigger_event(event_name, data);
        }
    }


