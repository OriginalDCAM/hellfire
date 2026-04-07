//
// Created by denzel on 07/10/2025.
//
#pragma once
#include "../EditorPanel.h"

namespace hellfire::editor {
class SceneHierarchyPanel : public EditorPanel {
public:
    void render_context_menu();

    void handle_shortcuts();

    void render() override;
private:
    void render_list();

    void render_add_entity_menu(EntityID parent_id = 0);

    void render_list_item(EntityID entity_id);

    EntityID entity_to_delete_ = {};

    std::unordered_map<EntityID, bool> entity_id_to_opened_nodes_;

    EntityID entity_to_rename_ = {};
};
}
