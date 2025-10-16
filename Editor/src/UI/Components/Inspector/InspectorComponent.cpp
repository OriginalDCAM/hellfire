//
// Created by denzel on 14/10/2025.
//

#include "InspectorComponent.h"

#include "imgui.h"
#include "hellfire/ecs/LightComponent.h"
#include "hellfire/ecs/RenderableComponent.h"
#include "hellfire/ecs/ScriptComponent.h"
#include "hellfire/ecs/TransformComponent.h"
#include "hellfire/ecs/components/MeshComponent.h"

namespace hellfire::editor {
    void InspectorComponent::render() {
        if (!context_->active_scene) return; // Don't show if there's no active scene selected
        auto active_scene = context_->active_scene;
        if (ImGui::Begin("Inspector")) {
            auto *selected_entity = active_scene->get_entity(context_->selected_entity_id);

            if (!selected_entity) {
                ImGui::TextDisabled("No entity selected");
                ImGui::End();
                return;
            }

            // Entity name
            static char name_buffer[256];
            strncpy_s(name_buffer, selected_entity->get_name().c_str(), 255);

            ImGui::Text("Name");
            ImGui::SameLine(120);
            if (ImGui::InputText("##EntityName", name_buffer, 256)) {
                selected_entity->set_name(name_buffer);
            }

            ImGui::Separator();

            // Transform Component (always present)
            if (auto transform = selected_entity->transform()) {
                render_transform_component(transform);
            }
            // Mesh Component
            if (auto *mesh = selected_entity->get_component<MeshComponent>()) {
                render_mesh_component(mesh);
            }
            // Renderable Component
            if (auto *renderable = selected_entity->get_component<RenderableComponent>()) {
                render_renderable_component(renderable);
            }
            // Light Component
            if (auto *light = selected_entity->get_component<LightComponent>()) {
                render_light_component(light);
            }
            // Camera Component
            if (auto *camera = selected_entity->get_component<CameraComponent>()) {
                render_camera_component(camera);
            }
            // Script Components
            for (auto script: selected_entity->get_script_components()) {
                render_script_component(script);
            }

            ImGui::End();
        }
    }

    void InspectorComponent::render_transform_component(TransformComponent *transform) {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto position = transform->get_position();
            auto rotation = transform->get_rotation();
            auto scale = transform->get_scale();

            ImGui::Indent();

            ImGui::Text("Position");
            ImGui::SameLine(120);
            ImGui::DragFloat3("##Position", &position[0]);
            transform->set_position(position);

            ImGui::Text("Rotation");
            ImGui::SameLine(120);
            ImGui::DragFloat3("##Rotation", &rotation[0]);
            transform->set_rotation(rotation);


            ImGui::Text("Scale");
            ImGui::SameLine(120);
            ImGui::DragFloat3("##Scale", &scale[0]);
            transform->set_scale(scale);

            ImGui::Unindent();
        }
    }

    void InspectorComponent::render_mesh_component(MeshComponent *mesh) {
        if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
            // TODO: Decide which inputs to use for this component
        }
    }


    void InspectorComponent::render_renderable_component(RenderableComponent *renderable) {
        if (ImGui::CollapsingHeader("Renderable", ImGuiTreeNodeFlags_DefaultOpen)) {
            std::shared_ptr<Material> material = renderable->get_material();

            ImGui::Indent();
            ImGui::Text("Material Properties");

            for (const auto &prop: material->get_properties() | std::views::values) {
                switch (prop.type) {
                    case Material::PropertyType::FLOAT: {
                        float float_val = std::get<float>(prop.value);
                        ImGui::Text("%s", prop.name.c_str());
                        ImGui::SameLine(120);
                        std::string prop_name = "##" + prop.name;
                        if (ImGui::DragFloat(prop_name.c_str(), &float_val)) {
                            material->set_property(prop.name, float_val);
                        }
                        break;
                    }
                    case Material::PropertyType::VEC2: {
                        glm::vec2 vec2_val = std::get<glm::vec2>(prop.value);
                        ImGui::Text("%s", prop.name.c_str());
                        ImGui::SameLine(120);
                        std::string prop_name = "##" + prop.name;
                        if (ImGui::DragFloat2(prop_name.c_str(), &vec2_val[0])) {
                            material->set_property(prop.name, vec2_val);
                        }
                        break;
                    }
                    case Material::PropertyType::COLOR3: {
                        glm::vec3 vec3_val = std::get<glm::vec3>(prop.value);
                        ImGui::Text("%s", prop.name.c_str());
                        ImGui::SameLine(120);
                        std::string prop_name = "##" + prop.name;
                        if (ImGui::ColorEdit3(prop_name.c_str(), &vec3_val[0])) {
                            material->set_property(prop.name, vec3_val, prop.type);
                        }
                        break;
                    }
                    case Material::PropertyType::VEC3: {
                        glm::vec3 vec3_val = std::get<glm::vec3>(prop.value);
                        ImGui::Text("%s", prop.name.c_str());
                        ImGui::SameLine(120);
                        std::string prop_name = "##" + prop.name;
                        if (ImGui::ColorEdit3(prop_name.c_str(), &vec3_val[0])) {
                            material->set_property(prop.name, vec3_val);
                        }
                    }
                    default: {
                        break;
                    }
                }
            }
            ImGui::Unindent();
        }
    }


    void InspectorComponent::render_light_component(LightComponent *light) {
        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
            const char *light_types[] = {"Directional", "Point"};
            int current_type = light->get_light_type();

            if (ImGui::Combo("Type", &current_type, light_types, 2)) {
                light->set_light_type(static_cast<LightComponent::LightType>(current_type));
            }

            auto color = light->get_color();
            if (ImGui::ColorEdit3("Color", &color[0])) {
                light->set_color(color);
            }

            // NOTE: Intensity set to a maximum of 10 because otherwise the picture will get too bright
            float intensity = light->get_intensity();
            if (ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, 10.0f)) {
                light->set_intensity(intensity);
            }

            switch (light->get_light_type()) {
                case LightComponent::DIRECTIONAL:
                    render_directional_light_component(light);
                    break;
                case LightComponent::SPOT:
                    // render_spot_light_component(light);
                    break;
                case LightComponent::POINT:
                    render_point_light_component(light);
                    break;
            }
        }
    }

    void InspectorComponent::render_directional_light_component(LightComponent *light) {
    }

    void InspectorComponent::render_point_light_component(LightComponent *light) {
    }


    void InspectorComponent::render_camera_component(CameraComponent *camera) {
    }

    void InspectorComponent::render_script_component(ScriptComponent *script) {
        if (ImGui::CollapsingHeader(script->get_class_name())) {
            for (const auto &property_info: script->get_properties()) {
                if (property_info.type == ScriptComponent::PropertyType::BOOL) {
                    auto* boolean_value = static_cast<bool*>(property_info.data_ptr);
                    if (ImGui::Checkbox(property_info.name.c_str(), boolean_value)) {
                        
                    }
                }
            }
        }
    }
}
