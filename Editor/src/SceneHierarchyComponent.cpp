//
// Created by denzel on 07/10/2025.
//
#include "SceneHierarchyComponent.h"

#include <imgui.h>

namespace hellfire::editor {
    void SceneHierarchyComponent::render() {
        ImVec2 window_size = ImGui::GetContentRegionAvail();
        ImGui::SetNextWindowSize(ImVec2(window_size.x/3, window_size.y/3), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Scene Hierarchy Panel")) {
            if (!context_->active_scene) {
                ImGui::Text("No scene selected");
                ImGui::End();
                return;
            }
            if (ImGui::TreeNode(context_->active_scene->get_name().c_str())) {
                for (EntityID entity_id : context_->active_scene->get_root_entities()) {
                    ImGui::PushID(entity_id);
                    
                    if (ImGui::TreeNode("", context_->active_scene->get_entity(entity_id)->get_name().c_str())) {
                        
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }
                // for (int i = 0; i < 5; i++) {
                //     // Use SetNextItemOpen() so set the default state of a node to be open. We could
                //     // also use TreeNodeEx() with the ImGuiTreeNodeFlags_DefaultOpen flag to achieve the same thing!
                //     if (i == 0)
                //         ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                //
                //     // Here we use PushID() to generate a unique base ID, and then the "" used as TreeNode id won't conflict.
                //     // An alternative to using 'PushID() + TreeNode("", ...)' to generate a unique ID is to use 'TreeNode((void*)(intptr_t)i, ...)',
                //     // aka generate a dummy pointer-sized value to be hashed. The demo below uses that technique. Both are fine.
                //     ImGui::PushID(i);
                //     if (ImGui::TreeNode("", "Child %d", i)) {
                //         ImGui::Text("blah blah");
                //         ImGui::SameLine();
                //         if (ImGui::SmallButton("button")) {
                //         }
                //         ImGui::TreePop();
                //     }
                //     ImGui::PopID();
                // }
                ImGui::TreePop();
            }
            ImGui::End();
        }
    }

    void SceneHierarchyComponent::render_list() {
    }

    void SceneHierarchyComponent::render_list_item() {
    }
}
