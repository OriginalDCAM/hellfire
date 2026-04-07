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
        EntityID  selected_entity_id = 0;
        std::vector<EntityID> selected_entity_ids;
        Scene* active_scene = nullptr;

        [[nodiscard]] bool has_selection() const { return selected_entity_id != 0; }

        [[nodiscard]] Entity* get_selected_entity() const {
            if (!active_scene || selected_entity_id == 0) return nullptr;
            return active_scene->get_entity(selected_entity_id);
        }

        void set_window_title(const std::string& title) {
            if (auto window = ServiceLocator::get_service<IWindow>()) {
                window->set_title(title + " - Hellfire Editor");
            }
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
