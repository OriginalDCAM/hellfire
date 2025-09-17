//
// Created by denzel on 21/08/2025.
//
#include "DCraft/Components/ScriptComponent.h"
#include "DCraft/Structs/Entity.h"
#include "DCraft/Components/TransformComponent.h"

#include <glm/detail/type_mat.hpp>

namespace DCraft {
    Entity::Entity(const std::string &name) {
        set_name(name);
        add_component<TransformComponent>();
    }

    void Entity::add(Entity *child) {
        if (const auto entity = std::find(children_.begin(), children_.end(), child); entity == children_.end()) {
            if (child->parent_ != nullptr) {
                child->parent_->remove(child);
            }

            children_.push_back(child);
            child->parent_ = this;

            if (initialized_) {
                child->setup_recursive();
            }
        }
    }

    void Entity::remove(Entity *child) {
        if (const auto entity = std::find(children_.begin(), children_.end(), child); entity != children_.end()) {
            // Remove from children's list
            children_.erase(entity);
            child->parent_ = nullptr;
        }
    }

    const std::vector<ScriptComponent *> &Entity::get_script_components() const {
        return script_components_;
    }

    void Entity::update_world_matrices() const {
        if (auto *transform = get_component<TransformComponent>()) {
            const glm::mat4 *parent_world = nullptr;
            if (parent_ && parent_->has_component<TransformComponent>()) {
                parent_world = &parent_->get_component<TransformComponent>()->get_world_matrix();
            }
            transform->update_world_matrix(parent_world);
        }

        // Update children
        for (const auto *child: children_) {
            child->update_world_matrices();
        }
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

    void Entity::broadcast_event(const std::string &event_name, void *data, const bool recursive) const {
        for (auto *script: script_components_) {
            script->trigger_event(event_name, data);
        }

        // Optionally broadcast to children
        if (recursive) {
            for (const auto *child: children_) {
                child->broadcast_event(event_name, data, true);
            }
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
