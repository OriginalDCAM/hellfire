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
#include "UI/ui.h"

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

            if (ui::Vec3Input("Position", &position, 0.1f)) {
                transform->set_position(position);
            }

            if (ui::Vec3Input("Rotation", &rotation, 0.1f)) {
                transform->set_position(rotation);
            }
            if (ui::Vec3Input("Scale", &scale, 0.1f)) {
                transform->set_position(scale);
            }

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
                        if (ui::FloatInput(prop.name, &float_val)) {
                            material->set_property(prop.name, float_val);
                        }
                        break;
                    }
                    case Material::PropertyType::TEXTURE: {
                        auto texture_ptr = std::get<Texture*>(prop.value);

                        // Construct the "use" property name
                        std::string use_property_name = "use" + ui::capitalize_first(prop.name);
    
                        // Retrieve the "use" flag from the material
                        bool property_enabled = material->get_property<bool>(use_property_name);

                        if (ui::TexturePropertyInput(prop.name, texture_ptr, property_enabled, material.get())) {
                            // Update both the texture and the "use" flag
                            material->set_property(prop.name, texture_ptr);
                            material->set_property(use_property_name, property_enabled);
                        }
                        break;
                    }
                    case Material::PropertyType::VEC2: {
                        glm::vec2 vec2_val = std::get<glm::vec2>(prop.value);
                        if (ui::Vec2Input(prop.name, &vec2_val)) {
                            material->set_property(prop.name, vec2_val);
                        }
                        break;
                    }
                    case Material::PropertyType::COLOR3: {
                        glm::vec3 vec3_val = std::get<glm::vec3>(prop.value);
                        if (ui::ColorPickerRGBInput(prop.name, &vec3_val)) {
                            material->set_property(prop.name, vec3_val, prop.type);
                        }
                        break;
                    }
                    case Material::PropertyType::VEC3: {
                        glm::vec3 vec3_val = std::get<glm::vec3>(prop.value);
                        if (ui::Vec3Input(prop.name, &vec3_val)) {
                            material->set_property(prop.name, vec3_val);
                        }
                        break;
                    }
                    case Material::PropertyType::BOOL: {
                        // bool bool_value = std::get<bool>(prop.value);
                        // if (ui::BoolInput(prop.name, &bool_value)) {
                        //     material->set_property(prop.name, bool_value);
                        // }
                        break;
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
            if (ui::ColorPickerRGBInput("Color", &color)) {
                light->set_color(color);
            }

            float intensity = light->get_intensity();
            if (ui::FloatInput("Intensity", &intensity, 0.1f, 0.0f, 100.0f)) {
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

    void InspectorComponent::render_script_component(const ScriptComponent *script) {
        if (ImGui::CollapsingHeader(script->get_class_name(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();
            for (const auto &prop: script->get_properties()) {
                if (prop.type == ScriptComponent::PropertyType::BOOL) {
                    auto *boolean_value = static_cast<bool *>(prop.data_ptr);
                    ui::BoolInput(prop.name, boolean_value);
                }
            }
            ImGui::Unindent();
        }
    }
}
