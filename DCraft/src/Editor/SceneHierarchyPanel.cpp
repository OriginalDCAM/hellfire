//
// Created by denzel on 09/04/2025.
//
#include "DCraft/Editor/SceneHierarchyPanel.h"

#include <imgui.h>

#include "DCraft/Graphics/Lights/DirectionalLight.h"
#include "DCraft/Graphics/Lights/Light.h"
#include "DCraft/Graphics/Lights/PointLight.h"
#include "DCraft/Graphics/Primitives/Shape3D.h"

namespace DCraft::Editor {
    void SceneHierarchyPanel::render_object_properties(Object3D *object) const {
        if (!object) return;
        ImGui::Begin("Properties");

        // Common properties for all Object3D types
        std::string name = object->get_name();
        char buffer[256];
        strcpy(buffer, name.c_str());
        if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
            object->set_name(buffer);
        }

        // Position, rotation, scale
        glm::vec3 position = object->get_position();
        if (ImGui::DragFloat3("Position", &position[0], 0.1)) {
            object->set_position(position);
        }

        glm::vec3 scale = object->get_scale();
        if (ImGui::DragFloat3("Scale", &scale[0], 0.1)) {
            object->set_scale(scale);
        }

        if (auto *light = dynamic_cast<Light *>(object)) {
            render_light_properties(light);
        } else if (auto *camera = dynamic_cast<Camera *>(object)) {
            render_camera_properties(camera);
        } else if (auto *shape = dynamic_cast<Shape3D *>(object)) {
            render_shape_properties(shape);
        }

        if (auto *mesh = object->get_mesh()) {
            render_mesh_properties(mesh);
        }

        ImGui::End();
    }

    void SceneHierarchyPanel::render() {
        render_menu_bar();

        ImGui::Begin("Scene Hierarchy");

        // Get scenes from scene manager
        auto &scenes = scene_manager_.get_objects(); // These are the Scene objects
        ImGui::Text("Number of scenes: %d", scenes.size());

        if (ImGui::BeginTable("ScenesTable", 1)) {
            ImGui::TableSetupColumn("Scenes and Objects");
            ImGui::TableHeadersRow();

            // Render each scene
            for (auto *scene: scenes) {
                if (!scene) continue;

                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                // Render the scene as a tree node
                if (ImGui::TreeNode(scene->get_name().c_str())) {
                    // Now render the objects in this scene
                    auto &sceneObjects = scene->get_children(); // Objects in the scene

                    for (auto *obj: sceneObjects) {
                        if (!obj) continue;

                        // Render each object in the scene
                        ImGui::Indent();
                        render_object_node(obj);
                        ImGui::Unindent();
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::EndTable();
        }

        ImGui::End();


        if (selected_node_) {
            render_object_properties(selected_node_);
        }
    }

    void SceneHierarchyPanel::render_object_node(Object3D *object) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

        if (object == selected_node_)
            flags |= ImGuiTreeNodeFlags_Selected;

        if (object->get_children().empty())
            flags |= ImGuiTreeNodeFlags_Leaf;

        bool open = ImGui::TreeNodeEx(object->get_name().c_str(), flags);

        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Remove")) {
                // Truncate any redoable commands
                if (current_command_index_ < command_history_.size() - 1) {
                    command_history_.resize(current_command_index_ + 1);
                }

                // Store removal command
                command_history_.push_back(std::make_unique<RemoveObjectCommand>(
                    scene_manager_, object->get_parent(), object));

                // Execute the command
                command_history_.back()->execute();

                // Update index to point to the new command
                current_command_index_ = command_history_.size() - 1;
            }
            ImGui::EndPopup();
        }

        // Handle selection
        if (ImGui::IsItemClicked())
            selected_node_ = object;

        if (open) {
            // Render children
            auto &children = object->get_children();
            for (auto *child: children) {
                render_object_node(child);
            }

            ImGui::TreePop();
        }
    }

    void SceneHierarchyPanel::undo() {
        if (current_command_index_ >= 0) {
            command_history_[current_command_index_]->undo();
            current_command_index_--;
        }
    }

    void SceneHierarchyPanel::redo() {
        if (current_command_index_ < command_history_.size() - 1) {
            current_command_index_++;
            command_history_[current_command_index_]->execute();
        }
    }

    void SceneHierarchyPanel::render_menu_bar() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "Ctrl+Z", false, current_command_index_ >= 0)) {
                    undo();
                }
                if (ImGui::MenuItem("Redo", "Ctrl+Y", false,
                                    current_command_index_ < command_history_.size() - 1)) {
                    redo();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void SceneHierarchyPanel::render_light_properties(Light *light) const {
        ImGui::SeparatorText("Light Properties");
        glm::vec3 color = light->get_color();
        if (ImGui::ColorPicker3("Light", &color[0], ImGuiColorEditFlags_Float)) {
            light->set_color(color);
        }

        float intensity = light->get_intensity();
        if (ImGui::DragFloat("Intensity", &intensity)) {
            light->set_intensity(intensity);
        }

        if (auto *dir_light = dynamic_cast<DirectionalLight *>(light)) {
            ImGui::SeparatorText("Directional Light Properties");
            glm::vec3 direction = dir_light->get_direction();
            if (ImGui::DragFloat3("Direction", &direction[0], 0.1, -1, 1)) {
                dir_light->set_direction(direction);
            }
        } else if (auto *point_light = dynamic_cast<PointLight *>(light)) {
            ImGui::SeparatorText("Point Light Properties");
            float range = point_light->get_range();
            if (ImGui::DragFloat("Range", &range, 0.1)) {
                point_light->set_range(range);
            }

            float attentuation = point_light->get_attenuation();
            if (ImGui::DragFloat("Attentuation", &attentuation, 0.5)) {
                point_light->set_attenuation(attentuation);
            }
        }
    }

    void SceneHierarchyPanel::render_camera_properties(Camera *camera) const {
        ImGui::SeparatorText("Camera Properties");

        if (auto *perspective_cam = dynamic_cast<PerspectiveCamera *>(camera)) {
            glm::vec3 target = perspective_cam->get_target();
            if (ImGui::DragFloat3("Target", &target[0], 0.1)) {
                perspective_cam->set_target(target.x, target.y, target.z);
            }
        }
    }

    void SceneHierarchyPanel::render_shape_properties(Shape3D *shape) const {
        Material *material = shape->get_material();
    }

    void SceneHierarchyPanel::render_mesh_properties(Mesh *mesh) const {
    }
}
