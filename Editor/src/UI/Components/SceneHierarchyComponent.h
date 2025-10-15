//
// Created by denzel on 07/10/2025.
//
#pragma once
#include "EditorComponent.h"

namespace hellfire::editor {
class SceneHierarchyComponent : public EditorComponent {
public:
    void render_context_menu();

    void render() override;
private:
    void render_list();

    void render_add_entity_menu(EntityID parent_id = 0);

    void render_list_item(EntityID entity_id);

    EntityID entity_to_delete_;
};
}
