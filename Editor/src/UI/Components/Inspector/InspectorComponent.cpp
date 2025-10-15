//
// Created by denzel on 14/10/2025.
//

#include "InspectorComponent.h"

#include "imgui.h"
#include "hellfire/ecs/LightComponent.h"
#include "hellfire/ecs/RenderableComponent.h"
#include "hellfire/ecs/ScriptComponent.h"
#include "hellfire/ecs/TransformComponent.h"

namespace hellfire::editor {
    void InspectorComponent::render() {
        if (!context_->active_scene) return; // Don't show if there's no active scene selected
        auto active_scene = context_->active_scene;
        if (ImGui::Begin("Inspector")) {
            if (auto* selected_entity = active_scene->get_entity(context_->selected_entity_id)) {
                if (auto transform = selected_entity->transform()) {
                    auto position = transform->get_position();
                    ImGui::DragFloat3("Position", &position[0]);
                    transform->set_position(position);

                    auto rotation = transform->get_rotation();
                    ImGui::DragFloat3("Rotation", &rotation[0]);
                    transform->set_rotation(rotation);

                    auto scale = transform->get_scale();
                    ImGui::DragFloat3("Scale", &scale[0]);
                    transform->set_scale(scale);
                }

                if (selected_entity->has_component<LightComponent>()) {
                    ImGui::Text("Light Component");
                }

                if (selected_entity->has_component<RenderableComponent>()) {
                    ImGui::Text("Renderable Component");
                }

                if (selected_entity->has_component<CameraComponent>()) {
                    
                }

                for (auto script_comp : selected_entity->get_script_components()) {
                    for (auto [label, value]: script_comp->get_bool_vars()) {
                        ImGui::Checkbox(label.c_str(), &value);
                        script_comp->set_bool(label, value);
                    }
                }

            }
            ImGui::End();
        }
    }
}
