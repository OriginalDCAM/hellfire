//
// Created by denzel on 21/08/2025.
//
#include "hellfire/ecs/ScriptComponent.h"
#include "Entity.h"
#include "hellfire/ecs/TransformComponent.h"

#include <glm/detail/type_mat.hpp>

namespace hellfire {
    const std::vector<ScriptComponent *> &Entity::get_script_components() const {
        return script_components_;
    }


    void Entity::initialize_scripts() const {
        for (auto *script: script_components_) {
            script->init();
        }
    }

    void Entity::update_scripts(const float delta_time) const {
        for (auto *script: script_components_) {
            script->update(delta_time);
        }
    }

    void Entity::cleanup_scripts() const {
        for (auto *script: script_components_) {
            script->remove();
        }
    }

    void Entity::broadcast_event(const std::string &event_name, void *data) const {
        for (auto *script : script_components_) {
            script->trigger_event(event_name, data);
        }
    }

    TransformComponent * Entity::transform() {
        auto *comp = get_component<TransformComponent>();
        if (!comp) {
            std::clog << "Warning: Entity '" << get_name()
                    << "' missing TransformComponent, auto-creating\n";
            comp = add_component<TransformComponent>();
        }
        return comp;
    }

    const TransformComponent * Entity::transform() const { return get_component<TransformComponent>(); }
}
