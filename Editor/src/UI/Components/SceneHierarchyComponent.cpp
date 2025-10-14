//
// Created by denzel on 07/10/2025.
//
#include "SceneHierarchyComponent.h"

#include <imgui.h>

#include "hellfire/ecs/RenderableComponent.h"
#include "hellfire/graphics/geometry/Cube.h"
#include "hellfire/graphics/geometry/Quad.h"
#include "hellfire/graphics/geometry/Sphere.h"

namespace hellfire::editor {
    void SceneHierarchyComponent::render_context_menu() {
        if (ImGui::BeginPopupContextWindow("HierarchyContextMenu")) {
            const auto active_scene = context_->active_scene;
            if (ImGui::MenuItem("Create Empty Entity")) {
                if (active_scene) {
                    active_scene->create_entity();
                }
            }
            if (ImGui::BeginMenu("Mesh")) {
                if (ImGui::MenuItem("Cube")) {
                    const EntityID new_cube_id = Cube::create(active_scene, "Cube", {});
                    context_->selected_entity_id = new_cube_id;
                }
                if (ImGui::MenuItem("Sphere")) {
                    const EntityID new_sphere_id = Sphere::create(active_scene, "Sphere", {});
                    context_->selected_entity_id = new_sphere_id;
                }
                if (ImGui::MenuItem("Quad")) {
                    const EntityID new_quad_id = Quad::create(active_scene, "Quad");
                    context_->selected_entity_id = new_quad_id;
                }
                ImGui::Separator(); 
                if (ImGui::MenuItem("Cylinder", 0, false, false)) { }
                if (ImGui::MenuItem("Stair", 0, false, false)) { }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }
    }

    void SceneHierarchyComponent::render() {
        const ImVec2 window_size = ImGui::GetContentRegionAvail();
        ImGui::SetNextWindowSize(ImVec2(window_size.x / 3, window_size.y / 3), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Scene Hierarchy Panel")) {
            if (context_->active_scene) {
                render_context_menu();
                render_list();
                
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

        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, false)) {
                // TODO: Implement duplicate
            }
            if (ImGui::MenuItem("Rename", "F2", false, false)) {
                // TODO: Open rename dialog
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Delete", "Del")) {
                context_->active_scene->destroy_entity(entity_id);
                if (context_->selected_entity_id == entity_id) {
                    context_->selected_entity_id = 0; // Deselect
                }
                ImGui::EndPopup();
                ImGui::PopID();
                return; // Don't render children of deleted entity
            }
            ImGui::EndPopup();
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
