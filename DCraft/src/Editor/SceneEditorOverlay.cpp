//
// Created by denzel on 09/04/2025.
//
#include "DCraft/Editor/SceneEditorOverlay.h"

#include <imgui.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "DCraft/Addons/ModelLoader.h"
#include "DCraft/Graphics/Lights/DirectionalLight.h"
#include "DCraft/Graphics/Lights/Light.h"
#include "DCraft/Graphics/Lights/PointLight.h"
#include "DCraft/Graphics/Materials/LambertMaterial.h"
#include "DCraft/Graphics/Materials/PhongMaterial.h"
#include "DCraft/Graphics/Primitives/Shape3D.h"
#include "DCraft/Editor/Components/MenuBarComponent.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace DCraft::Editor {
    void SceneEditorOverlay::render_object_properties(Object3D *object) const {
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

        glm::vec3 rotation = object->get_rotation();
        if (ImGui::DragFloat3("Rotation", &rotation[0], 1.0f, -180.0f, 180.0f)) {
            // Normalize angles to -180 to 180 range for better user experience
            for (int i = 0; i < 3; i++) {
                while (rotation[i] > 180.0f) rotation[i] -= 360.0f;
                while (rotation[i] < -180.0f) rotation[i] += 360.0f;
            }
            object->set_rotation(rotation);
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

    void SceneEditorOverlay::render() {
        MenuBarComponent menu_bar_component;
        menu_bar_component.init(scene_manager_, command_history_, current_command_index_);
        menu_bar_component.render();

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

    void SceneEditorOverlay::render_object_node(Object3D *object) {
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

    void SceneEditorOverlay::render_light_properties(Light *light) const {
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

    void SceneEditorOverlay::render_camera_properties(Camera *camera) const {
        ImGui::SeparatorText("Camera Properties");

        float movement_speed = camera->get_movement_speed();
        if (ImGui::DragFloat("Movement speed", &movement_speed, 0.25)) {
            camera->set_movement_speed(movement_speed);
        }

        if (auto *perspective_cam = dynamic_cast<PerspectiveCamera *>(camera)) {
            glm::vec3 target = perspective_cam->get_target();
            if (ImGui::DragFloat3("Target", &target[0], 0.1)) {
                perspective_cam->set_target(target.x, target.y, target.z);
            }
        }
    }

    void SceneEditorOverlay::render_shape_properties(Shape3D *shape) const {
        ImGui::Begin("Material Editor");

        if (!shape) {
            ImGui::Text("No shape selected");
            ImGui::End();
            return;
        }

        Material *material = shape->get_material();
        if (!material) {
            ImGui::Text("Selected shape has no material");
            ImGui::End();
            return;
        }

        // Material name
        std::string name = material->get_name();
        char buffer[256];
        strcpy(buffer, name.c_str());
        if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
            material->set_name(buffer);
        }

        ImGui::Separator();

        // Display material type
        ImGui::Text("Material Type: %s", typeid(*material).name());

        // Specific material properties based on type
        if (auto lambert = dynamic_cast<LambertMaterial *>(material)) {
            // Diffuse color editing
            glm::vec3 diffuse = lambert->get_diffuse_color();
            float diffuse_color[3] = {diffuse.r, diffuse.g, diffuse.b};
            if (ImGui::ColorEdit3("Diffuse Color", diffuse_color)) {
                lambert->set_diffuse_color(glm::vec3(diffuse_color[0], diffuse_color[1], diffuse_color[2]));
            }

            // Texture section
            ImGui::Separator();
            ImGui::Text("Diffuse Texture");

            // Show current texture if it exists
            if (lambert->has_diffuse_texture()) {
                Texture *tex = lambert->get_diffuse_texture();
                if (tex && tex->get_id() > 0) {
                    // Show texture preview
                    ImGui::Image(reinterpret_cast<ImTextureID>((void *) (uintptr_t) tex->get_id()), ImVec2(150, 150));

                    if (ImGui::Button("Remove Texture")) {
                        // lambert->remove_diffuse_texture();
                    }
                }
            } else {
                ImGui::Text("No texture");
                if (ImGui::Button("Add Texture...")) {
                    std::vector<Utility::FileFilter> scene_filters = {
                        {"Texture Files", "*.png;*.jpg;*.jpeg"},
                        {"All Files", "*.*"}
                    };

                    std::string filepath = Utility::FileDialog::open_file(scene_filters);
                    fs::path absolute_path = filepath;
                    fs::path assets_dir = "assets";

                    // Check if assets_dir is part of the path
                    std::string path_str = absolute_path.string();
                    size_t pos = path_str.find(assets_dir.string());

                    if (pos != std::string::npos) {
                        std::string relative_path = path_str.substr(pos);
                        lambert->set_texture(relative_path, TextureType::DIFFUSE);
                    } else {
                        lambert->set_texture(filepath, TextureType::DIFFUSE);
                    }
                }
            }
        } else if (auto phong = dynamic_cast<PhongMaterial *>(material)) {
            // Phong Material Properties
            ImGui::Text("Phong Material Properties");


            // Ambient color
            glm::vec3 ambient = phong->get_ambient_color();
            float ambient_color[3] = {ambient.r, ambient.g, ambient.b};
            if (ImGui::ColorEdit3("Ambient Color", ambient_color)) {
                phong->set_ambient_color(glm::vec3(ambient_color[0], ambient_color[1], ambient_color[2]));
            }

            // Diffuse color
            glm::vec3 diffuse = phong->get_diffuse_color();
            float diffuse_color[3] = {diffuse.r, diffuse.g, diffuse.b};
            if (ImGui::ColorEdit3("Diffuse Color", diffuse_color)) {
                phong->set_diffuse_color(glm::vec3(diffuse_color[0], diffuse_color[1], diffuse_color[2]));
            }

            // Specular color
            glm::vec3 specular = phong->get_specular_color();
            float specular_color[3] = {specular.r, specular.g, specular.b};
            if (ImGui::ColorEdit3("Specular Color", specular_color)) {
                phong->set_specular_color(glm::vec3(specular_color[0], specular_color[1], specular_color[2]));
            }

            // Shininess
            float shininess = phong->get_shininess();
            if (ImGui::SliderFloat("Shininess", &shininess, 1.0f, 128.0f, "%.1f")) {
                phong->set_shininess(shininess);
            }
        } else {
            // Base Material Properties or unknown material type
            ImGui::Text("Basic Material Properties");
            // Add any properties common to all materials
        }

        ImGui::Separator();

        // Material change section
        if (ImGui::CollapsingHeader("Change Material Type")) {
            if (ImGui::Button("Lambert Material")) {
                // Create new Lambert material
                auto new_material = new LambertMaterial(material->get_name());
                // Copy any common properties from old material
                // ...

                // Replace material
                shape->set_material(new_material);
                // Handle memory cleanup for old material if needed
            }

            if (ImGui::Button("Phong Material")) {
                // Create new Phong material
                auto new_material = new PhongMaterial(material->get_name());
                // Copy any common properties from old material
                // ...

                // Replace material
                shape->set_material(new_material);
                // Handle memory cleanup for old material if needed
            }

            // Add buttons for other material types as needed
        }

        ImGui::End();
    }


    void SceneEditorOverlay::render_mesh_properties(Mesh *mesh) const {
    }
}
