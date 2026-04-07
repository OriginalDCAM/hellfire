//
// Created by denzel on 09/04/2025.
//
#include "DCraft/Editor/SceneEditorOverlay.h"

#include <imgui.h>

#include "DCraft/Addons/ModelLoader.h"
#include "DCraft/Graphics/Lights/DirectionalLight.h"
#include "DCraft/Graphics/Lights/Light.h"
#include "DCraft/Graphics/Lights/PointLight.h"
#include "DCraft/Graphics/Primitives/Shape3D.h"

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

    void SceneEditorOverlay::undo() {
        if (current_command_index_ >= 0) {
            command_history_[current_command_index_]->undo();
            current_command_index_--;
        }
    }

    void SceneEditorOverlay::redo() {
        if (current_command_index_ < command_history_.size() - 1) {
            current_command_index_++;
            command_history_[current_command_index_]->execute();
        }
    }

    void SceneEditorOverlay::setup_docking_space() {
        // Set up the dockspace
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    
        // Make the parent window full-screen
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
        // Begin the main dockspace window
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar(2);
    
        // Add a dockspace inside this window
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    
        // Optional menu bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                // Add menu items
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    
        // Create your specific windows
        // These will be dockable into the dockspace
    
        // Scene Hierarchy window - default to left side
        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
        ImGui::Begin("Scene Hierarchy");
        // Scene hierarchy content here
        ImGui::End();
    
        // Inspector window - default to right side
        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
        ImGui::Begin("Inspector");
        // Inspector content here
        ImGui::End();
    
        // End the dockspace window
        ImGui::End();
    }

    void SceneEditorOverlay::render_menu_bar() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Import Model...")) {
                    // For a real application, you'd use a file dialog
                    // But for your assignment, you could use a hardcoded path for testing
                    std::string filepath = "assets/models/teapot.obj";  // Replace with your test model
                    import_model(filepath);
                }
                ImGui::Separator();
                ImGui::EndMenu();
            }
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

    

    void SceneEditorOverlay::import_model(const std::string &filepath) {
        Object3D* model = Addons::ModelLoader::load(filepath, scene_manager_.get_active_scene());

        if (model) {
            // Position the model at a visible location
            model->set_position(glm::vec3(0.0f, 2.0f, 0.0f));
        
            scene_manager_.get_active_scene()->add(model);
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
        Material *material = shape->get_material();
    }

    void SceneEditorOverlay::render_mesh_properties(Mesh *mesh) const {
    }

}
