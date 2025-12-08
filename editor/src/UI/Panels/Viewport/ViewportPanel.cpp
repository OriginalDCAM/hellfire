//
// Created by denzel on 13/10/2025.
//

#include "ViewportPanel.h"

#include "IconsFontAwesome6.h"
#include "hellfire/graphics/renderer/Renderer.h"

#include "imgui.h"
#include "ImGuizmo.h"
#include "SceneCameraScript.h"
#include "hellfire/core/Time.h"
#include "hellfire/platform/windows_linux/GLFWWindow.h"
#include "UI/ui.h"

namespace hellfire::editor {
    ViewportPanel::ViewportPanel() {
        engine_renderer_ = ServiceLocator::get_service<Renderer>();
        assert(engine_renderer_ != nullptr);
        create_editor_camera();
    }

    Framebuffer* ViewportPanel::get_picking_fbo() {
        if (!picking_fbo_) {
            picking_fbo_ = std::make_unique<Framebuffer>();
        }
        return picking_fbo_.get();
    }

    ViewportPanel::~ViewportPanel() {
        destroy_editor_camera();
    }

    void ViewportPanel::create_editor_camera() {
        editor_camera_ = new Entity(INVALID_ENTITY, "Editor Camera"); // set entityID to 0 

        editor_camera_->add_component<TransformComponent>();

        // Add Camera Component
        auto *cam_component = editor_camera_->add_component<CameraComponent>(
            CameraType::PERSPECTIVE

        );
        cam_component->set_aspect_ratio(16.0f / 9.0f);
        cam_component->set_fov(70.0f);
        cam_component->set_clip_planes(0.1f, 2000.0f);
        cam_component->set_mouse_sensitivity(0.1f);

        // Set initial position and orientation
        editor_camera_->transform()->set_position(20.0f, 30.0f, 50.0f);
        cam_component->look_at(glm::vec3(0.0f));

        // Add Camera Control Script
        editor_camera_->add_component<SceneCameraScript>(40.0f);
        std::cout << "Editor camera created for viewport" << std::endl;
    }

    void ViewportPanel::destroy_editor_camera() {
        if (editor_camera_) {
            delete editor_camera_;
            editor_camera_ = nullptr;
            std::cout << "Editor camera destroyed" << std::endl;
        }
    }

    void ViewportPanel::update_camera_control() {
        if (!editor_camera_) return;

        if (context_->active_scene) {
            context_->active_scene->set_default_camera(editor_camera_->get_id());
        }

        // Camera is active when right-clicking viewport
        const bool viewport_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
        const bool right_mouse_pressed = ImGui::IsMouseClicked(ImGuiMouseButton_Right);
        const bool right_mouse_released = ImGui::IsMouseReleased(ImGuiMouseButton_Right);

        if (right_mouse_pressed && viewport_hovered) {
            camera_active_ = true;
            ImGui::GetIO().WantCaptureMouse = false; // don't let ImGui eat camera input
        }
        if (right_mouse_released) {
            camera_active_ = false;
        }

        // Enable/Disable camera script
        if (auto *camera_script = editor_camera_->get_component<SceneCameraScript>()) {
            camera_script->set_enabled(camera_active_);

            // Check whether the camera script is enabled to call the update method, only when the state camera active is set within this component
            if (camera_script->is_enabled()) {
                camera_script->update(Time::delta_time);
            }
        }

        // Hide cursor when camera is active
        const auto window = ServiceLocator::get_service<IWindow>();
        GLFWwindow *glfwWindow = static_cast<GLFWwindow *>(window->get_native_handle());
        if (camera_active_) {
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
        } else {
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }


    void TextCentered(const std::string &text) {
        const auto viewport_size = ImGui::GetContentRegionAvail();
        const auto text_width = ImGui::CalcTextSize(text.c_str()).x;
        ImGui::SetCursorPos(ImVec2((viewport_size.x - text_width) / 2, viewport_size.y / 2));
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("%s", text.c_str());
        ImGui::SetWindowFontScale(1.0f);
    }

    void ViewportPanel::render_viewport_image() {
        if (!engine_renderer_) return;

        ImVec2 viewport_size = ImGui::GetContentRegionAvail();

        // Store last valid size
        if (viewport_size.x > 0 && viewport_size.y > 0) {
            viewport_size_ = viewport_size;
        } else {
            viewport_size = viewport_size_;
        }

        // Resize framebuffer if needed
        static ImVec2 last_size = {0, 0};
        const auto current_time = static_cast<float>(ImGui::GetTime());

        if (constexpr float RESIZE_DELAY = 0.016f; (viewport_size.x != last_size.x || viewport_size.y != last_size.y) &&
                                                   (current_time - last_resize_time_) > RESIZE_DELAY) {
            engine_renderer_->resize_main_framebuffer(
                static_cast<uint32_t>(viewport_size.x),
                static_cast<uint32_t>(viewport_size.y)
            );

            last_size = viewport_size;
            last_resize_time_ = current_time;

            // Update editor camera aspect ratio
            if (editor_camera_) {
                if (auto *cam = editor_camera_->get_component<CameraComponent>()) {
                    cam->set_aspect_ratio(viewport_size.x / viewport_size.y);
                }
            }
        }
        // Render using editor camera
        const uint32_t scene_texture = engine_renderer_->get_main_output_texture();

        if (!context_->active_scene) {
            ImGui::SetCursorPos(ImVec2(
                (viewport_size.x - ImGui::CalcTextSize("No Active Scene").x) / 2,
                viewport_size.y / 2
            ));
            ImGui::Text("No Active Scene");
        } else if (!editor_camera_) {
            ImGui::SetCursorPos(ImVec2(
                (viewport_size.x - ImGui::CalcTextSize("Editor Camera Missing!").x) / 2,
                viewport_size.y / 2
            ));
            ImGui::Text("Editor Camera Missing!");
        } else {
            ImGui::Image(scene_texture,
                         viewport_size, ImVec2(0, 1), ImVec2(1, 0));
        }
    }


    void ViewportPanel::render_transform_gizmo() {
        if (auto *selected_entity = context_->active_scene->get_entity(context_->selected_entity_id)) {
            const auto entity_transform = selected_entity->transform();

            // Set ImGuizmo to draw in this window
            ImGuizmo::SetDrawlist();
            const ImVec2 image_size = ImGui::GetItemRectSize();
            const ImVec2 image_pos = ImGui::GetItemRectMin();
            ImGuizmo::SetRect(image_pos.x, image_pos.y, image_size.x, image_size.y);

            const auto camera = get_editor_camera();
            const auto camera_comp = camera->get_component<CameraComponent>();

            glm::mat4 view_matrix = camera_comp->get_view_matrix();
            glm::mat4 projection_matrix = camera_comp->get_projection_matrix();

            // Get entity's transform matrix
            glm::mat4 transform_matrix = context_->active_scene->has_parent(selected_entity->get_id())
                                             ? entity_transform->get_world_matrix()
                                             : entity_transform->get_local_matrix();


            if (!camera_active_ && !is_using_gizmo_) {
                if (ImGui::IsKeyPressed(ImGuiKey_G)) {
                    current_operation_ = ImGuizmo::TRANSLATE;
                } else if (ImGui::IsKeyPressed(ImGuiKey_R)) {
                    current_operation_ = ImGuizmo::ROTATE;
                } else if (ImGui::IsKeyPressed(ImGuiKey_S)) {
                    current_operation_ = ImGuizmo::SCALE;
                }
                current_mode_ = context_->active_scene->has_parent(selected_entity->get_id())
                                    ? ImGuizmo::WORLD
                                    : ImGuizmo::LOCAL;
            }

            Manipulate(glm::value_ptr(view_matrix), glm::value_ptr(projection_matrix), current_operation_,
                       current_mode_, glm::value_ptr(transform_matrix));

            // If the gizmo was used, update the entity's transform
            if (is_using_gizmo_ = ImGuizmo::IsUsing(); is_using_gizmo_) {
                glm::vec3 translation, rotation, scale;

                if (context_->active_scene->has_parent(context_->selected_entity_id)) {
                    auto parent_entity_id = context_->active_scene->get_parent(context_->selected_entity_id);
                    auto parent_entity = context_->active_scene->get_entity(parent_entity_id);
                    glm::mat4 parent_world_matrix = parent_entity->transform()->get_world_matrix();
                    glm::mat4 local_matrix = glm::inverse(parent_world_matrix) * transform_matrix;

                    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(local_matrix), glm::value_ptr(translation),
                                                          glm::value_ptr(rotation), glm::value_ptr(scale));
                } else {
                    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform_matrix), glm::value_ptr(translation),
                                                          glm::value_ptr(rotation), glm::value_ptr(scale));
                }

                // Update entity's transform
                entity_transform->set_position(translation);
                entity_transform->set_rotation(rotation); // rotation is in degrees
                entity_transform->set_scale(scale);
            }
        }
    }

    void ViewportPanel::handle_object_picking() {
        if (!context_->active_scene) return;
        if (!engine_renderer_) return;
        
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !is_using_gizmo_) {
            const ImVec2 mouse_pos = ImGui::GetMousePos();
            const ImVec2 viewport_pos = ImGui::GetItemRectMin();

            // Convert to viewport-relative coordinates
            const int mouse_x = static_cast<int>(mouse_pos.x - viewport_pos.x);
            const int mouse_y = static_cast<int>(mouse_pos.y - viewport_pos.y);

            const uint32_t picked_entity_id = pick_object_at_mouse(mouse_x, mouse_y);
            context_->selected_entity_id = picked_entity_id;
        }
    }

    void ViewportPanel::render() {
        const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
        const ImVec2 default_size = ImVec2(main_viewport->Size.x / 1.5f, main_viewport->Size.y / 1.5f);
        const ImVec2 default_pos = ImVec2(
            main_viewport->Pos.x + (main_viewport->Size.x - default_size.x) * 0.5f,
            main_viewport->Pos.y + (main_viewport->Size.y - default_size.y) * 0.5f
        );

        ImGui::SetNextWindowSize(default_size, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(default_pos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(320, 180), ImVec2(FLT_MAX, FLT_MAX));

        const std::string window_name = context_->active_scene
                                            ? ICON_FA_EYE " " + context_->active_scene->get_name()
                                            : "Viewport";

        if (ui::Window window{window_name}) {
            if (!context_->active_scene) return;
            // Store the viewport bound for outside usage
            viewport_pos_ = ImGui::GetWindowPos();
            viewport_size_ = ImGui::GetWindowSize();
            viewport_hovered_ = ImGui::IsWindowHovered();

            render_viewport_image();
            render_transform_gizmo();
            handle_object_picking();

            update_camera_control();

            if (viewport_size_.x > 256) {
                render_viewport_stats_overlay();
            }
        }
    }


    void ViewportPanel::render_viewport_stats_overlay() const {
        // Position overlay in top-left corner with padding
        constexpr float padding = 10.0f;
        ImGui::SetNextWindowPos(ImVec2(viewport_pos_.x + padding, viewport_pos_.y + padding + ImGui::GetFrameHeight()));
        ImGui::SetNextWindowBgAlpha(0.35f);

        constexpr ImGuiWindowFlags overlay_flags =
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoFocusOnAppearing |
                ImGuiWindowFlags_NoNav |
                ImGuiWindowFlags_NoMove;

        if (ImGui::Begin("##ViewportStats", nullptr, overlay_flags)) {
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("Frame Time: %.2f ms", 1000.0f / ImGui::GetIO().Framerate);

            if (context_->active_scene) {
                ImGui::Separator();
                ImGui::Text("Entities: %zu", context_->active_scene->get_entity_count());
            }

            if (editor_camera_) {
                ImGui::Separator();
                const glm::vec3 pos = editor_camera_->transform()->get_position();
                ImGui::Text("Cam Pos: %.1f, %.1f, %.1f", pos.x, pos.y, pos.z);
            }
        }
        ImGui::End();
    }

    uint32_t ViewportPanel::pick_object_at_mouse(const int mouse_x, const int mouse_y) {
        if (!engine_renderer_) return 0;
        
        const uint32_t object_id_texture = engine_renderer_->get_object_id_texture();
        if (object_id_texture == 0) return 0;

        // Get viewport dimensions
        const ImVec2 viewport_size = viewport_size_;

        const int tex_x = mouse_x;
        const int tex_y = static_cast<int>(viewport_size.y) - mouse_y - 1;

        // Bounds check
        if (tex_x < 0 || tex_x >= static_cast<int>(viewport_size.x) ||
            tex_y < 0 || tex_y >= static_cast<int>(viewport_size.y)) {
            return 0;
        }

        const uint32_t pixel_data = get_picking_fbo()->read_pixel_from_texture(object_id_texture, tex_x, tex_y);

        return pixel_data;
    }
};
