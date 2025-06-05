//
// Created by denzel on 09/04/2025.
//
#include "DCraft/Editor/SceneEditorOverlay.h"

#include <imgui.h>

#include "DCraft/Addons/ModelLoader.h"
#include "DCraft/Graphics/Lights/DirectionalLight.h"
#include "DCraft/Graphics/Lights/Light.h"
#include "DCraft/Graphics/Lights/PointLight.h"
#include "DCraft/Graphics/Primitives/MeshRenderer.h"
#include "DCraft/Editor/Components/MenuBarComponent.h"
#include "DCraft/Graphics/Renderer.h"
#include "DCraft/Structs/Camera.h"  

#include <filesystem>
#include <iostream>  

namespace fs = std::filesystem;

namespace DCraft::Editor {
    
    void SceneEditorOverlay::render() {
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        render_viewport();
        render_scene_hierarchy();

        if (selected_node_) {
            render_object_properties(selected_node_);
        }
    }

    void SceneEditorOverlay::render_viewport() {
        ImGui::Begin("Scene Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        viewport_focused_ = ImGui::IsWindowFocused();

        ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();

        if (viewport_panel_size.x < 50.0f) viewport_panel_size.x = 50.0f;
        if (viewport_panel_size.y < 50.0f) viewport_panel_size.y = 50.0f;

        bool size_changed = (viewport_size_.x != viewport_panel_size.x || viewport_size_.y != viewport_panel_size.y);

        viewport_size_ = viewport_panel_size;

        if (size_changed) {
            std::cout << "Viewport resized to " << viewport_size_.x << "x" << viewport_size_.y << std::endl;
            viewport_needs_update_ = true;

            if (renderer_) {
                renderer_->resize_scene_framebuffer(
                    static_cast<uint32_t>(viewport_size_.x),
                    static_cast<uint32_t>(viewport_size_.y)
                );
            }
        }

        if (!active_scene_) {
            active_scene_ = scene_manager_.get_active_scene();
        }

        bool needs_render = check_if_render_needed();

        if (active_scene_ && active_camera_ && renderer_ && needs_render) {
            renderer_->render_to_texture(*active_scene_, *active_camera_, 
                                       static_cast<uint32_t>(viewport_size_.x), 
                                       static_cast<uint32_t>(viewport_size_.y));
            
            cached_scene_texture_ = renderer_->get_scene_texture();
            viewport_needs_update_ = false;
            scene_changed_ = false;

            last_camera_position_ = active_camera_->get_position();
            last_camera_rotation_ = active_camera_->get_rotation();
        }

        if (cached_scene_texture_ > 0) {
            ImTextureID tex_id = (ImTextureID)(intptr_t)cached_scene_texture_;
            ImGui::Image(tex_id, viewport_size_, ImVec2(0, 1), ImVec2(1, 0));
        
            if (ImGui::IsItemHovered()) {
                handle_viewport_input();
            }
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "No scene texture available");
        }
    
        // Display FPS and render info
        ImGui::SetCursorPos(ImVec2(10, ImGui::GetWindowHeight() - 50));
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%.0fx%.0f", viewport_size_.x, viewport_size_.y);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%.1f FPS", ImGui::GetIO().Framerate);
    
        ImGui::End();
    }

    bool SceneEditorOverlay::check_if_render_needed() {
        if (!active_camera_) return false;
        
        // Always render if explicitly marked dirty
        if (viewport_needs_update_ || scene_changed_) {
            return true;
        }
        
        // Check if camera moved
        glm::vec3 current_pos = active_camera_->get_position();
        if (glm::distance(current_pos, last_camera_position_) > 0.001f) {
            return true;
        }
        
        // Check camera rotation
        glm::vec3 current_rot = active_camera_->get_rotation();
        if (glm::distance(current_rot, last_camera_rotation_) > 0.001f) {
            return true;
        }
        
        // Check if mouse is being used for look controls
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && viewport_focused_) {
            return true;
        }
        
        // Check if any movement keys are pressed
        if (viewport_focused_) {
            if (ImGui::IsKeyDown(ImGuiKey_W) || ImGui::IsKeyDown(ImGuiKey_A) || 
                ImGui::IsKeyDown(ImGuiKey_S) || ImGui::IsKeyDown(ImGuiKey_D) ||
                ImGui::IsKeyDown(ImGuiKey_Q) || ImGui::IsKeyDown(ImGuiKey_E)) {
                return true;
            }
        }
        
        return false;
    }

    void SceneEditorOverlay::mark_scene_dirty() {
        scene_changed_ = true;
    }

    void SceneEditorOverlay::mark_viewport_dirty() {
        viewport_needs_update_ = true;
    }

    void SceneEditorOverlay::render_scene_hierarchy() {
        ImGui::Begin("Scene Hierarchy");

        auto &scenes = scene_manager_.get_objects();
        ImGui::Text("Number of scenes: %d", static_cast<int>(scenes.size()));

        if (ImGui::BeginTable("ScenesTable", 1)) {
            ImGui::TableSetupColumn("Scenes and Objects");
            ImGui::TableHeadersRow();

            for (auto *scene: scenes) {
                if (!scene) continue;

                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                // Render the scene as a tree node
                bool node_open = ImGui::TreeNode(scene->get_name().c_str());

                if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                    ImGui::OpenPopup(("SceneContextMenu_" + scene->get_name()).c_str());
                }

                // Create context menu
                if (ImGui::BeginPopup(("SceneContextMenu_" + scene->get_name()).c_str())) {
                    if (scene_manager_.get_active_scene() != scene) {
                        if (ImGui::MenuItem("Activate Scene")) {
                            scene_manager_.set_active_scene(static_cast<Scene *>(scene));
                            active_scene_ = scene;
                            mark_viewport_dirty();
                        }
                    }
                    if (ImGui::MenuItem("Remove Scene")) {
                        bool is_active = scene_manager_.get_active_scene() == scene;

                        Scene *new_active_scene = nullptr;
                        if (is_active) {
                            auto &all_scenes = scene_manager_.get_objects();
                            for (auto *potential_scene: all_scenes) {
                                if (potential_scene && potential_scene != scene) {
                                    new_active_scene = static_cast<Scene *>(potential_scene);
                                    break;
                                }
                            }
                        }

                        auto remove_command = std::make_unique<RemoveObjectCommand>(
                            scene_manager_, scene->get_parent(), scene);
                        execute_command(std::move(remove_command));

                        if (is_active && new_active_scene) {
                            scene_manager_.set_active_scene(new_active_scene);
                            active_scene_ = new_active_scene;
                        } else if (is_active) {
                            active_scene_ = nullptr;
                        }

                        mark_scene_dirty(); 

                        ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                        continue;
                    }
                    ImGui::EndPopup();
                }
                if (node_open) {
                    auto &scene_objects = scene->get_children();

                    for (auto *obj: scene_objects) {
                        if (!obj) continue;

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
    }

    void SceneEditorOverlay::render_object_node(Object3D *object) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

        if (object == selected_node_)
            flags |= ImGuiTreeNodeFlags_Selected;

        if (object->get_children().empty())
            flags |= ImGuiTreeNodeFlags_Leaf;

        bool open = ImGui::TreeNodeEx(object->get_name().c_str(), flags);

        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Remove Object")) {
                auto remove_command = std::make_unique<RemoveObjectCommand>(
                    scene_manager_, object->get_parent(), object);
                execute_command(std::move(remove_command));
                mark_scene_dirty(); 
            }
            ImGui::EndPopup();
        }

        if (ImGui::IsItemClicked())
            selected_node_ = object;

        if (open) {
            auto &children = object->get_children();
            for (auto *child: children) {
                render_object_node(child);
            }
            ImGui::TreePop();
        }
    }

    void SceneEditorOverlay::handle_viewport_input() {
        if (!active_camera_) return;

        ImGuiIO &io = ImGui::GetIO();
        bool camera_changed = false;

        // Handle camera rotation with right mouse
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            ImVec2 mouse_delta = io.MouseDelta;
        
            if (std::abs(mouse_delta.x) > 0.1f || std::abs(mouse_delta.y) > 0.1f) {
                active_camera_->process_mouse_movement(mouse_delta.x, -mouse_delta.y, true);
                camera_changed = true;
            }
        
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
        }
        
        if (viewport_focused_) {
            float original_speed = active_camera_->get_movement_speed();
            float speed_multiplier = 1.0f;
            
            if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift)) {
                speed_multiplier = 3.0f;
            }
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                speed_multiplier = 0.3f;
            }
            
            active_camera_->set_movement_speed(original_speed * speed_multiplier);

            if (ImGui::IsKeyDown(ImGuiKey_W)) {
                active_camera_->process_keyboard(FORWARD, io.DeltaTime);
                camera_changed = true;
            }
            if (ImGui::IsKeyDown(ImGuiKey_S)) {
                active_camera_->process_keyboard(BACKWARD, io.DeltaTime);
                camera_changed = true;
            }
            if (ImGui::IsKeyDown(ImGuiKey_A)) {
                active_camera_->process_keyboard(LEFT, io.DeltaTime);
                camera_changed = true;
            }
            if (ImGui::IsKeyDown(ImGuiKey_D)) {
                active_camera_->process_keyboard(RIGHT, io.DeltaTime);
                camera_changed = true;
            }
            if (ImGui::IsKeyDown(ImGuiKey_Q)) {
                active_camera_->process_keyboard(DOWN, io.DeltaTime);
                camera_changed = true;
            }
            if (ImGui::IsKeyDown(ImGuiKey_E)) {
                active_camera_->process_keyboard(UP, io.DeltaTime);
                camera_changed = true;
            }
            
            active_camera_->set_movement_speed(original_speed);
        }

        if (io.MouseWheel != 0.0f) {
            active_camera_->process_mouse_scroll(io.MouseWheel);
            camera_changed = true;
        }

        if (camera_changed) {
            mark_viewport_dirty();
        }
    }

    void SceneEditorOverlay::execute_command(std::unique_ptr<EditorCommand> command) {
        if (current_command_index_ < static_cast<int>(command_history_.size()) - 1) {
            command_history_.resize(current_command_index_ + 1);
        }

        command->execute();
        command_history_.push_back(std::move(command));
        current_command_index_ = static_cast<int>(command_history_.size()) - 1;
    }

    void SceneEditorOverlay::undo_last_command() {
        if (current_command_index_ >= 0 && current_command_index_ < static_cast<int>(command_history_.size())) {
            command_history_[current_command_index_]->undo();
            current_command_index_--;
            mark_scene_dirty();  
        }
    }

    void SceneEditorOverlay::redo_next_command() {
        if (current_command_index_ + 1 < static_cast<int>(command_history_.size())) {
            current_command_index_++;
            command_history_[current_command_index_]->execute();
            mark_scene_dirty();
        }
    }

    void SceneEditorOverlay::render_object_properties(Object3D *object) const {
        if (!object) return;
        ImGui::Begin("Properties");

        std::string name = object->get_name();
        char buffer[256];
        strcpy(buffer, name.c_str());
        if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
            object->set_name(buffer);
        }

        glm::vec3 position = object->get_position();
        if (ImGui::DragFloat3("Position", &position[0], 0.1f)) {
            object->set_position(position);
            const_cast<SceneEditorOverlay*>(this)->mark_scene_dirty(); 
        }

        glm::vec3 scale = object->get_scale();
        if (ImGui::DragFloat3("Scale", &scale[0], 0.1f)) {
            object->set_scale(scale);
            const_cast<SceneEditorOverlay*>(this)->mark_scene_dirty();
        }

        glm::vec3 rotation = object->get_rotation();
        if (ImGui::DragFloat3("Rotation", &rotation[0], 1.0f, -180.0f, 180.0f)) {
            for (int i = 0; i < 3; i++) {
                while (rotation[i] > 180.0f) rotation[i] -= 360.0f;
                while (rotation[i] < -180.0f) rotation[i] += 360.0f;
            }
            object->set_rotation(rotation);
            const_cast<SceneEditorOverlay*>(this)->mark_scene_dirty();
        }

        if (auto *light = dynamic_cast<Light *>(object)) {
            render_light_properties(light);
        } else if (auto *camera = dynamic_cast<Camera *>(object)) {
            render_camera_properties(camera);
        } else if (auto *shape = dynamic_cast<MeshRenderer *>(object)) {
            render_shape_properties(shape);
        }

        if (auto *mesh = object->get_mesh()) {
            render_mesh_properties(mesh);
        }

        ImGui::End();
    }

    void SceneEditorOverlay::render_light_properties(Light *light) const {
        ImGui::SeparatorText("Light Properties");
        glm::vec3 color = light->get_color();
        if (ImGui::ColorPicker3("Light", &color[0], ImGuiColorEditFlags_Float)) {
            light->set_color(color);
            const_cast<SceneEditorOverlay*>(this)->mark_scene_dirty(); 
        }

        float intensity = light->get_intensity();
        if (ImGui::DragFloat("Intensity", &intensity)) {
            light->set_intensity(intensity);
            const_cast<SceneEditorOverlay*>(this)->mark_scene_dirty(); 
        }

        if (auto *dir_light = dynamic_cast<DirectionalLight *>(light)) {
            ImGui::SeparatorText("Directional Light Properties");
            glm::vec3 direction = dir_light->get_direction();
            if (ImGui::DragFloat3("Direction", &direction[0], 0.1f, -1.0f, 1.0f)) {
                dir_light->set_direction(direction);
                const_cast<SceneEditorOverlay*>(this)->mark_scene_dirty();
            }
        } else if (auto *point_light = dynamic_cast<PointLight *>(light)) {
            ImGui::SeparatorText("Point Light Properties");
            float range = point_light->get_range();
            if (ImGui::DragFloat("Range", &range, 0.1f)) {
                point_light->set_range(range);
                const_cast<SceneEditorOverlay*>(this)->mark_scene_dirty();
            }

            float attenuation = point_light->get_attenuation();
            if (ImGui::DragFloat("Attenuation", &attenuation, 0.01f)) {
                point_light->set_attenuation(attenuation);
                const_cast<SceneEditorOverlay*>(this)->mark_scene_dirty();
            }
        }
    }

    void SceneEditorOverlay::render_camera_properties(Camera *camera) const {
        ImGui::SeparatorText("Camera Properties");

        float movement_speed = camera->get_movement_speed();
        if (ImGui::DragFloat("Movement speed", &movement_speed, 0.25f)) {
            camera->set_movement_speed(movement_speed);
            // Camera speed doesn't affect rendering, so no need to mark dirty
        }

        if (auto *perspective_cam = dynamic_cast<PerspectiveCamera *>(camera)) {
            glm::vec3 target = perspective_cam->get_target();
            if (ImGui::DragFloat3("Target", &target[0], 0.1f)) {
                perspective_cam->set_target(target.x, target.y, target.z);
                const_cast<SceneEditorOverlay*>(this)->mark_viewport_dirty(); 
            }
        }
    }

    void SceneEditorOverlay::render_shape_properties(MeshRenderer *shape) const {
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

        std::string name = material->get_name();
        char buffer[256];
        strcpy(buffer, name.c_str());
        if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
            // material->set_name(buffer);
        }

        ImGui::Separator();
        ImGui::Text("Material Type: %s", typeid(*material).name());
        ImGui::Text("Basic Material Properties");
        ImGui::Text("Diffuse Texture");

        ImGui::End();
    }

    void SceneEditorOverlay::render_mesh_properties(Mesh *mesh) const {
    }
    
}