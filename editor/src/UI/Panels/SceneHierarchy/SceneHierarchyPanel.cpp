//
// Created by denzel on 07/10/2025.
//

#include "SceneHierarchyPanel.h"

#include <imgui.h>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "IconsFontAwesome6.h"
#include "hellfire/core/InputManager.h"
#include "hellfire/ecs/RenderableComponent.h"
#include "hellfire/graphics/geometry/Cube.h"
#include "hellfire/graphics/geometry/Quad.h"
#include "hellfire/graphics/geometry/Sphere.h"
#include "hellfire/graphics/lighting/DirectionalLight.h"
#include "hellfire/graphics/lighting/PointLight.h"
#include "hellfire/scene/CameraFactory.h"

namespace hellfire::editor {
void SceneHierarchyPanel::render_context_menu() {
    if (ImGui::BeginPopupContextWindow("HierarchyContextMenu")) {
        render_add_entity_menu();
        ImGui::EndPopup();
    }
}

void SceneHierarchyPanel::handle_shortcuts() {
    if (!context_->selected_entity_id) return;
    auto im = ServiceLocator::get_service<InputManager>();

    if (im->is_key_pressed(GLFW_KEY_DELETE)) {
        entity_to_delete_ = context_->selected_entity_id;
    }

    if (im->is_key_pressed(GLFW_KEY_F2)) {
        entity_to_rename_ = context_->selected_entity_id;
    }
}

void SceneHierarchyPanel::render() {
    const ImVec2 window_size = ImGui::GetContentRegionAvail();
    ImGui::SetNextWindowSize(ImVec2(window_size.x / 3, window_size.y / 3),
                             ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Scene Hierarchy Panel")) {
        if (context_->active_scene) {
            handle_shortcuts();
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 6.0f));
            ImGui::PushStyleColor(ImGuiCol_Button,
                                  ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                  ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                                  ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
            // Add button at the top
            if (ImGui::Button(ICON_FA_PLUS "##AddEntity", ImVec2(40, 30))) {
                ImGui::OpenPopup("AddEntityPopup");
            }

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(2);

            // Tooltip for button
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Add Entity");
            }

            // Popup menu for adding entities
            if (ImGui::BeginPopup("AddEntityPopup")) {
                render_add_entity_menu();
                ImGui::EndPopup();
            }
            ImGui::Spacing();
            render_context_menu();
            render_list();
        } else {
            ImGui::Text("No scene selected");
        }
    }
    ImGui::End();

    if (entity_to_delete_ != 0) {
        context_->active_scene->destroy_entity(entity_to_delete_);
        entity_to_delete_ = 0;
    }
}

void SceneHierarchyPanel::render_list() {
    const std::string& active_scene_name = context_->active_scene->get_name();

    // Set the flags for the tree node
    constexpr ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_DefaultOpen |     // Open by default
        ImGuiTreeNodeFlags_SpanAvailWidth |  // Full width
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_DrawLinesToNodes |  // Draw lines to each child node
        ImGuiTreeNodeFlags_FramePadding;

    if (ImGui::TreeNodeEx(active_scene_name.c_str(), flags)) {
        for (const EntityID entity_id :
             context_->active_scene->get_root_entities()) {
            render_list_item(entity_id);
        }

        ImGui::TreePop();
    }
}

void SceneHierarchyPanel::render_add_entity_menu(const EntityID parent_id) {
    const auto active_scene = context_->active_scene;
    bool has_parent = parent_id != 0;

    EntityID new_entity_id = 0;
    if (ImGui::MenuItem("Empty Entity")) {
        if (active_scene) {
            new_entity_id = active_scene->create_entity();
            context_->selected_entity_id = new_entity_id;
        }
    }

    if (ImGui::BeginMenu("Mesh")) {
        if (ImGui::MenuItem("Cube")) {
            new_entity_id = Cube::create(active_scene, "Cube", {});
        }
        if (ImGui::MenuItem("Sphere")) {
            new_entity_id = Sphere::create(active_scene, "Sphere", {});
        }
        if (ImGui::MenuItem("Quad")) {
            new_entity_id = Quad::create(active_scene, "Quad");
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Cylinder", nullptr, false, false)) {
        }
        if (ImGui::MenuItem("Stair", nullptr, false, false)) {
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Lights")) {
        if (ImGui::MenuItem("Directional")) {
            new_entity_id =
                DirectionalLight::create(active_scene, "Directional Light");
        }
        if (ImGui::MenuItem("Point")) {
            new_entity_id = PointLight::create(active_scene, "Point Light");
        }
        if (ImGui::MenuItem("Spot", nullptr, false, false)) {
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Cameras")) {
        if (ImGui::MenuItem("Perspective")) {
            new_entity_id = OrthographicCamera::create(
                active_scene, "Orthographic Camera", 1.0, 16.0f / 9.0f);
        }
        if (ImGui::MenuItem("Orthographic")) {
            new_entity_id =
                PerspectiveCamera::create(active_scene, "Perspective Camera");
        }
        ImGui::EndMenu();
    }
    if (new_entity_id != 0 && active_scene) {
        context_->selected_entity_id = new_entity_id;
        if (has_parent) active_scene->set_parent(new_entity_id, parent_id);
    }
}

void SceneHierarchyPanel::render_list_item(const EntityID entity_id) {
    ImGui::PushID(static_cast<int>(
        entity_id));  // Push the id onto the stack to guarantee uniqueness for
                      // the list items.

    Entity* entity = context_->active_scene->get_entity(entity_id);
    if (!entity) {
        ImGui::PopID();
        return;
    }

    const std::string& entity_name = entity->get_name();

    // Check if this entity has children
    const auto children = context_->active_scene->get_children(entity_id);
    const bool has_children = !children.empty();

    // Highlight selected entity
    const bool is_selected = context_->selected_entity_id == entity_id;

    // Set up tree node flags
    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DrawLinesToNodes;

    if (is_selected) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (!has_children) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    // Styles
    ImGui::PushStyleVar(ImGuiStyleVar_TreeLinesRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

    // Render the tree node
    std::string display_name = ICON_FA_CUBES " " + entity_name;
    bool node_open = false;

    if (entity_to_rename_ == entity_id) {
        flags |= ImGuiTreeNodeFlags_DefaultOpen;
        flags |= ImGuiTreeNodeFlags_AllowOverlap;

        ImGui::SetNextItemOpen(true, ImGuiCond_Always);

        // Unique treenode for this entity (no visible label)
        node_open = ImGui::TreeNodeEx(
            (std::string("##entity_") + std::to_string(entity_id)).c_str(),
            flags);

        // Inputfield on the same line as the treenode
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::SetKeyboardFocusHere();  // Cursor automatically gets put in the
                                        // inputfield

        static char name_buffer[256];
        strncpy_s(name_buffer, entity->get_name().c_str(), 256);

        // Unique id for the inputfield, to prevent double id conflicts
        std::string input_id = "##RenameField_" + std::to_string(entity_id);

        if (ImGui::InputText(input_id.c_str(), name_buffer, 256,
                             ImGuiInputTextFlags_EnterReturnsTrue |
                                 ImGuiInputTextFlags_AutoSelectAll)) {
            entity->set_name(name_buffer);
            entity_to_rename_ = 0;
        }

        // Leave rename modus when field is not focussed anymore
        if (!ImGui::IsItemActive()) {
            entity_to_rename_ = 0;
        }
    } else {
        node_open = ImGui::TreeNodeEx(display_name.c_str(), flags);
    }

    ImGui::PopStyleVar(2);

    if (ImGui::IsItemClicked()) {
        context_->selected_entity_id = entity_id;
    }

    if (ImGui::BeginDragDropSource()) {
        ImGui::Text("%s", display_name.c_str());
        ImGui::SetDragDropPayload("ENTITY_ID", &entity_id, sizeof(EntityID));
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        ImGuiDragDropFlags target_flags = 0;
        target_flags |= ImGuiDragDropFlags_None;
        if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload("ENTITY_ID", target_flags)) {
            const EntityID* child_id = static_cast<EntityID*>(payload->Data);
            context_->active_scene->set_parent(*child_id, entity_id);
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::BeginPopupContextItem()) {
        render_add_entity_menu(entity_id);
        ImGui::Separator();
        if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, false)) {
            // TODO: Implement duplicate
        }
        if (ImGui::MenuItem("Rename", "F2", false, true)) {
            entity_to_rename_ = entity_id;
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Delete", "Del")) {
            entity_to_delete_ = entity_id;
            if (context_->selected_entity_id == entity_id) {
                context_->selected_entity_id = 0;  // Deselect
            }
        }

        ImGui::EndPopup();
    }

    // Recursively render children if node is open
    if (node_open && has_children) {
        for (const EntityID child_id : children) {
            render_list_item(child_id);
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}
}  // namespace hellfire::editor
