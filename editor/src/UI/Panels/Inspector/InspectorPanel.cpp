//
// Created by denzel on 14/10/2025.
//

#include "InspectorPanel.h"

#include "imgui.h"
#include "hellfire/ecs/LightComponent.h"
#include "hellfire/ecs/RenderableComponent.h"
#include "hellfire/ecs/ScriptComponent.h"
#include "hellfire/ecs/TransformComponent.h"
#include "hellfire/ecs/CameraComponent.h"
#include "hellfire/ecs/components/MeshComponent.h"
#include "hellfire/graphics/geometry/Cube.h"
#include "hellfire/graphics/geometry/Sphere.h"
#include "Scenes/RotateScript.h"
#include "UI/ui.h"

namespace hellfire::editor {
    void InspectorPanel::render_add_component_context_menu(Entity *selected_entity) {
        if (ImGui::BeginPopupContextWindow("AddComponentPopup")) {
            const bool has_light = selected_entity->has_component<LightComponent>();
            if (ImGui::MenuItem("Light", nullptr, false, !has_light)) {
                selected_entity->add_component<LightComponent>();
            }
            const bool has_renderable = selected_entity->has_component<RenderableComponent>();
            if (ImGui::MenuItem("Renderable", nullptr, false, !has_renderable)) {
                selected_entity->add_component<RenderableComponent>();
            }
            if (ImGui::BeginMenu("Script")) {
                if (ImGui::MenuItem("Rotate Script")) {
                    if (selected_entity->has_component<RotateScript>()) {
                    } else {
                        selected_entity->add_component<RotateScript>();
                    }
                }
                ImGui::EndMenu();
            }
            bool has_mesh = selected_entity->has_component<MeshComponent>();
            if (ImGui::MenuItem("Mesh", nullptr, false, !has_mesh)) {
                selected_entity->add_component<MeshComponent>();
            }
            bool has_camera = selected_entity->has_component<CameraComponent>();
            if (ImGui::MenuItem("Camera", nullptr, false, !has_camera)) {
                selected_entity->add_component<CameraComponent>();
            }
            ImGui::EndPopup();
        }
    }

    void InspectorPanel::render() {
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

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::BeginGroup();
            ImGui::SameLine(120, 1);
            render_add_component_context_menu(selected_entity);
            if (ImGui::Button("Add Component", ImVec2(320, 20))) {
                ImGui::OpenPopup("AddComponentPopup");
            }
            ImGui::EndGroup();

            ImGui::End();
        }
    }

    void InspectorPanel::render_transform_component(TransformComponent *transform) {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto position = transform->get_position();
            auto rotation = transform->get_rotation();
            auto scale = transform->get_scale();

            ImGui::Indent();

            float minValue = std::numeric_limits<float>::lowest();
            float maxValue = std::numeric_limits<float>::max();
            if (ui::vec3_input("Position", &position, 0.1f, minValue, maxValue)) {
                transform->set_position(position);
            }

            if (ui::vec3_input("Rotation", &rotation, 0.1f, minValue, maxValue)) {
                transform->set_rotation(rotation);
            }
            if (ui::vec3_input("Scale", &scale, 0.1f, minValue, maxValue)) {
                transform->set_scale(scale);
            }

            ImGui::Unindent();
        }
    }

    void InspectorPanel::render_mesh_component(MeshComponent *mesh) {
        if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
            // TODO: Decide which inputs to use for this component
            // Probably allow for the mesh to be changed?
            if (mesh->get_source() == MeshSource::INTERNAL) {
                ImGui::Text("Internal Mesh Source");
                const auto *mesh_types_labels = "Cube\0Sphere\0Quad\0";
                auto current_type = 0;

                if (ui::combo_box_int("Mesh", mesh_types_labels, &current_type)) {
                }
            }
        }
    }


    void InspectorPanel::render_renderable_component(RenderableComponent *renderable) {
        if (!ImGui::CollapsingHeader("Renderable", ImGuiTreeNodeFlags_DefaultOpen)) {
            return;
        }

        ImGui::Indent();
        std::shared_ptr<Material> material = renderable->get_material();
        // TODO: When serialization is implemented, handle this with being able to load materials from files
        if (!material) {
            ImGui::TextDisabled("No material assigned");
            return;
        }

        // Show material name and type
        if (material->has_custom_shader()) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "[Custom]");
        }
        ImGui::Separator();

        // Standard Properties
        if (!material->has_custom_shader()) {
            if (ImGui::CollapsingHeader("Colors", ImGuiTreeNodeFlags_DefaultOpen)) {
                // Diffuse
                auto diffuse = material->get_property<glm::vec3>(
                    MaterialConstants::DIFFUSE_COLOR);
                if (ui::color_picker_rgb_input("Diffuse", &diffuse)) {
                    material->set_diffuse_color(diffuse);
                }

                // Specular
                auto specular = material->get_property<glm::vec3>(
                    MaterialConstants::SPECULAR_COLOR);
                if (ui::color_picker_rgb_input("Specular", &specular)) {
                    material->set_specular_color(specular);
                }
            }

            if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
                // Opacity
                auto opacity = material->get_property<float>(
                    MaterialConstants::OPACITY);
                if (ui::float_input("Opacity", &opacity, 0.1f, 0.0f, 1.0f)) {
                    material->set_opacity(opacity);
                }

                // Shininess
                auto shininess = material->get_property<float>(
                    MaterialConstants::SHININESS);
                if (ui::float_input("Shininess", &shininess, 1.0f, 0.0f, 512.0f)) {
                    material->set_shininess(shininess);
                }

                // Metallic
                auto metallic = material->get_property<float>(
                    MaterialConstants::METALLIC);
                if (ui::float_input("Metallic", &metallic, 0.1f, 0.0f, 1.0f)) {
                    material->set_metallic(metallic);
                }

                // Roughness
                auto roughness = material->get_property<float>(
                    MaterialConstants::ROUGHNESS);
                if (ui::float_input("Roughness", &roughness, 0.1f, 0.0f, 1.0f)) {
                    material->set_roughness(roughness);
                }
            }

            if (ImGui::CollapsingHeader("UV Transform")) {
                // UV tiling
                auto tiling = material->get_property<glm::vec2>(
                    MaterialConstants::UV_TILING);
                if (ui::vec2_input("Tiling", &tiling)) {
                    material->set_uv_tiling(tiling);
                }

                // UV Offset
                auto offset = material->get_property<glm::vec2>(
                    MaterialConstants::UV_OFFSET);
                if (ui::vec2_input("Offset", &offset)) {
                    material->set_uv_offset(offset);
                }
            }
        }

        // Textures
        if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen)) {
            for (const auto &slot: TextureSlotManager::get_all_slots()) {
                ui::texture_slot_widget(material.get(), slot);
                ImGui::Spacing();
            }
        }
        ImGui::Unindent();
    }


    void InspectorPanel::render_light_component(LightComponent *light) {
        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
            const auto *light_types_labels = "Directional\0Point\0";
            int current_type = light->get_light_type();

            if (ui::combo_box_int("Light Type", light_types_labels, &current_type)) {
                light->set_light_type(static_cast<LightComponent::LightType>(current_type));
            }

            auto color = light->get_color();
            if (ui::color_picker_rgb_input("Color", &color)) {
                light->set_color(color);
            }

            float intensity = light->get_intensity();
            if (ui::float_input("Intensity", &intensity, 0.1f, 0.0f, 100.0f)) {
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

    void InspectorPanel::render_directional_light_component(LightComponent *light) {
        if (ImGui::CollapsingHeader("Light Specific Properties")) {
        }
    }

    void InspectorPanel::render_point_light_component(LightComponent *light) {
        if (ImGui::CollapsingHeader("Light Specific Properties")) {
            float range = light->get_range();
            if (ui::float_input("Range", &range)) {
                light->set_range(range);
            }

            float attenuation = light->get_attenuation();
            if (ui::float_input("Attenuation", &attenuation)) {
                light->set_attenuation(attenuation);
            }
        }
    }


    void InspectorPanel::render_camera_component(CameraComponent *camera) {
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        }
    }

    void InspectorPanel::render_script_component(const ScriptComponent *script) {
        if (ImGui::CollapsingHeader(script->get_class_name(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();
            for (const auto &prop: script->get_properties()) {
                if (prop.type == ScriptComponent::PropertyType::BOOL) {
                    auto *boolean_value = static_cast<bool *>(prop.data_ptr);
                    ui::bool_input(prop.name, boolean_value);
                }

                if (prop.type == ScriptComponent::PropertyType::VEC3) {
                    auto *vec3_value = static_cast<glm::vec3 *>(prop.data_ptr);
                    ui::vec3_input(prop.name, vec3_value);
                }
            }
            ImGui::Unindent();
        }
    }
}
