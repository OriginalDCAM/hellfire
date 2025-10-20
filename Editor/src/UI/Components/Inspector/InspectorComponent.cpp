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

    void InspectorComponent::render_mesh_component(MeshComponent *mesh) {
        if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
            // TODO: Decide which inputs to use for this component
            // Probably allow for the mesh to be changed?
            if (mesh->get_source() == MeshSource::INTERNAL) {
                ImGui::Text("Internal Mesh Source");
            }
        }
    }


    void InspectorComponent::render_renderable_component(RenderableComponent *renderable) {
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
        ImGui::Text("Material: %s", material->get_name().c_str());
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
                float opacity = material->get_property<float>(
                    MaterialConstants::OPACITY);
                if (ui::float_input("Opacity", &opacity, 0.1f, 0.0f, 1.0f)) {
                    material->set_opacity(opacity);
                }

                // Shininess
                float shininess = material->get_property<float>(
                    MaterialConstants::SHININESS);
                if (ui::float_input("Shininess", &shininess, 1.0f, 0.0f, 512.0f)) {
                    material->set_shininess(shininess);
                }

                // Metallic
                float metallic = material->get_property<float>(
                    MaterialConstants::METALLIC);
                if (ui::float_input("Metallic", &metallic, 0.1f, 0.0f, 1.0f)) {
                    material->set_metallic(metallic);
                }

                // Roughness
                float roughness = material->get_property<float>(
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


    void InspectorComponent::render_light_component(LightComponent *light) {
        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
            const char *light_types[] = {"Directional", "Point"};
            int current_type = light->get_light_type();

            if (ImGui::Combo("Type", &current_type, light_types, 2)) {
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

    void InspectorComponent::render_directional_light_component(LightComponent *light) {
        if (ImGui::CollapsingHeader("Light Specific Properties")) {
            
        }
    }

    void InspectorComponent::render_point_light_component(LightComponent *light) {
        if (ImGui::CollapsingHeader("Light Specific Properties")) {
            
        }
    }


    void InspectorComponent::render_camera_component(CameraComponent *camera) {
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            
        }
    }

    void InspectorComponent::render_script_component(const ScriptComponent *script) {
        if (ImGui::CollapsingHeader(script->get_class_name() , ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();
            for (const auto &prop: script->get_properties()) {
                if (prop.type == ScriptComponent::PropertyType::BOOL) {
                    auto *boolean_value = static_cast<bool *>(prop.data_ptr);
                    ui::bool_input(prop.name, boolean_value);
                }

                if (prop.type == ScriptComponent::PropertyType::VEC3) {
                    auto *vec3_value = static_cast<glm::vec3*>(prop.data_ptr);
                    ui::vec3_input(prop.name, vec3_value);
                }
            }
            ImGui::Unindent();
        }
    }
}
