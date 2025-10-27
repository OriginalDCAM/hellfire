//
// Created by denzel on 01/10/2025.
//

#pragma once
#include "hellfire/ecs/Entity.h"
#include "hellfire/platform/IWindow.h"
#include "hellfire/scene/Scene.h"
#include "hellfire/utilities/ServiceLocator.h"

namespace hellfire::editor {
    class EditorContext {
    public:
        EntityID  selected_entity_id = INVALID_ENTITY;
        std::vector<EntityID> selected_entity_ids;
        Scene* active_scene = nullptr;

        EntityID get_primary_selection() const { 
            return selected_entity_ids.empty() ? 0 : selected_entity_ids.front(); 
        }

        void set_window_title(const std::string& title) {
            if (auto window = ServiceLocator::get_service<IWindow>()) {
                window->set_title(title + " - Hellfire Editor");
            }
        }
    };
    
    class EditorPanel {
    protected:
        EditorContext* context_ = nullptr;
    public:
        virtual ~EditorPanel() = default;

        virtual void set_context(EditorContext* ctx) { context_ = ctx; }
        virtual void render() = 0;
        virtual void on_entity_selected(Entity* entity) {}
    };
}
