//
// Created by denzel on 01/10/2025.
//

#pragma once
#include "hellfire/ecs/Entity.h"
#include "hellfire/scene/Scene.h"

namespace hellfire::editor {
    class EditorContext {
    public:
        EntityID  selected_entity_id = 0;
        std::vector<EntityID> selected_entity_ids;
        Scene* active_scene = nullptr;

        [[nodiscard]] bool has_selection() const { return selected_entity_id != 0; }

        [[nodiscard]] Entity* get_selected_entity() const {
            if (!active_scene || selected_entity_id == 0) return nullptr;
            return active_scene->get_entity(selected_entity_id);
        }
    };
    
    class EditorComponent {
    protected:
        EditorContext* context_ = nullptr;
    public:
        virtual ~EditorComponent() = default;

        virtual void set_context(EditorContext* ctx) { context_ = ctx; }
        virtual void render() = 0;
        virtual void on_entity_selected(Entity* entity) {}
    };
}
