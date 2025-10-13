//
// Created by denzel on 07/10/2025.
//
#include "SceneHierarchyComponent.h"

#include <imgui.h>

namespace hellfire::editor {
    void SceneHierarchyComponent::render() {
        const ImVec2 window_size = ImGui::GetContentRegionAvail();
        ImGui::SetNextWindowSize(ImVec2(window_size.x / 3, window_size.y / 3), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Scene Hierarchy Panel")) {
            if (context_->active_scene) {
                render_list();
                if (ImGui::BeginPopupContextWindow("HierarchyContextMenu")) {
                    if (ImGui::MenuItem("Create Empty Entity")) {
                        // Create new entity
                    }
                    if (ImGui::BeginMenu("Mesh")) {
                        // Paste entity
                        if (ImGui::MenuItem("Cube")) { }
                        if (ImGui::MenuItem("Sphere")) { }
                        if (ImGui::MenuItem("Plane")) { }
                        if (ImGui::MenuItem("Cylinder", 0, false, false)) { }
                        if (ImGui::MenuItem("Stair", 0, false, false)) { }
                        ImGui::EndMenu();
                    }
                    ImGui::EndPopup();
                }
                
            } else {
                ImGui::Text("No scene selected");
            }
        }
        ImGui::End();
    }

    void SceneHierarchyComponent::render_list() {
        const std::string &active_scene_name = context_->active_scene->get_name();
        if (ImGui::TreeNode(active_scene_name.c_str())) {
            for (const EntityID entity_id: context_->active_scene->get_root_entities()) {
                render_list_item(entity_id);
            }
            
            ImGui::TreePop();
        }
    }

    void SceneHierarchyComponent::render_list_item(EntityID entity_id) {
        ImGui::PushID(entity_id); // Push the id onto the stack to guarantee uniqueness for the list items.

        const Entity *entity = context_->active_scene->get_entity(entity_id);
        if (!entity) {
            ImGui::PopID();
            return;
        }

        const std::string &entity_name = entity->get_name();

        // Check if this entity has children
        const auto children = context_->active_scene->get_children(entity_id);
        const bool has_children = !children.empty();

        // Highlight selected entity
        const bool is_selected = context_->selected_entity_id == entity_id;

        // Set up tree node flags
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                   ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                   ImGuiTreeNodeFlags_SpanAvailWidth;

        if (is_selected) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        if (!has_children) {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        // Render the tree node
        bool node_open = ImGui::TreeNodeEx(entity_name.c_str(), flags);

        if (ImGui::IsItemClicked()) {
            context_->selected_entity_id = entity_id;
        }

        // Recursively render children if node is open
        if (node_open && has_children) {
            for (const EntityID child_id: children) {
                render_list_item(child_id);
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }
}
