//
// Created by denzel on 13/10/2025.
//

#include "ViewportComponent.h"
#include "hellfire/graphics/renderer/Renderer.h"

#include "imgui.h"
#include "SceneCameraScript.h"
#include "hellfire/platform/windows_linux/GLFWWindow.h"

namespace hellfire::editor {
    ViewportComponent::ViewportComponent() {
        create_editor_camera();
    }

    ViewportComponent::~ViewportComponent() {
        destroy_editor_camera();
    }

    void ViewportComponent::create_editor_camera() {
        editor_camera_ = new Entity(INVALID_ENTITY, "Editor Camera");

        editor_camera_->add_component<TransformComponent>();

        // Add Camera Component
        auto *cam_component = editor_camera_->add_component<CameraComponent>(
            CameraType::PERSPECTIVE

        );
        cam_component->set_aspect_ratio(16.0f / 9.0f);
        cam_component->set_fov(70.0f);
        cam_component->set_clip_planes(0.1f, 1000.0f);
        cam_component->set_mouse_sensitivity(0.1f);

        // Set initial position and orientation
        editor_camera_->transform()->set_position(20.0f, 30.0f, 200.0f);
        cam_component->look_at(glm::vec3(0.0f));

        // Add Camera Control Script
        editor_camera_->add_component<SceneCameraScript>(12.5f);
        std::cout << "Editor camera created for viewport" << std::endl;
    }

    void ViewportComponent::destroy_editor_camera() {
        if (editor_camera_) {
            delete editor_camera_;
            editor_camera_ = nullptr;
            std::cout << "Editor camera destroyed" << std::endl;
        }
    }

    void ViewportComponent::update_camera_control() {
        if (!editor_camera_) return;

        if (context_->active_scene) {
            context_->active_scene->set_active_camera(editor_camera_->get_id());
        }

        // Camera is active when right-clicking viewport
        bool viewport_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
        bool right_mouse_pressed  = ImGui::IsMouseClicked(ImGuiMouseButton_Right);
        bool right_mouse_released = ImGui::IsMouseReleased(ImGuiMouseButton_Right);

        if (right_mouse_pressed && viewport_hovered) {
            camera_active_ = true;
            ImGui::GetIO().WantCaptureMouse = false; // don't let ImGui eat camera input
        }
        if (right_mouse_released) {
            camera_active_ = false;
        }

        // Enable/Disable camera script
        auto* camera_script = editor_camera_->get_component<SceneCameraScript>();
        if (camera_script) {
            camera_script->set_enabled(camera_active_);
        }

        if (camera_script->is_enabled()) {
            camera_script->update(0.1f);
        }
        
        // Hide cursor when camera is active
        auto window = ServiceLocator::get_service<IWindow>();
        GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->get_native_handle());
        if (camera_active_) {
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
        } else {
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }


    void TextCentered(const std::string &text) {
        auto viewport_size = ImGui::GetContentRegionAvail();
        auto text_width = ImGui::CalcTextSize(text.c_str()).x;
        ImGui::SetCursorPos(ImVec2((viewport_size.x - text_width) / 2, viewport_size.y / 2));
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text(text.c_str());
        ImGui::SetWindowFontScale(1.0f);
    }

    void ViewportComponent::render_viewport_image() {
        auto* renderer = ServiceLocator::get_service<Renderer>();
        if (!renderer) return;

        ImVec2 viewport_size = ImGui::GetContentRegionAvail();

        // Store last valid size
        if (viewport_size.x > 0 && viewport_size.y > 0) {
            viewport_size_ = viewport_size;
        } else {
            viewport_size = viewport_size_;
        }

        // Resize framebuffer if needed
        static ImVec2 last_size = {0, 0};
        float current_time = static_cast<float>(ImGui::GetTime());
        const float RESIZE_DELAY = 0.016f;

        if ((viewport_size.x != last_size.x || viewport_size.y != last_size.y) &&
    (current_time - last_resize_time_) > RESIZE_DELAY) {
            renderer->resize_scene_framebuffer(
                static_cast<uint32_t>(viewport_size.x),
                static_cast<uint32_t>(viewport_size.y)
            );
        
            last_size = viewport_size;
            last_resize_time_ = current_time;

            // Update editor camera aspect ratio
            if (editor_camera_) {
                auto* cam = editor_camera_->get_component<CameraComponent>();
                if (cam) {
                    cam->set_aspect_ratio(viewport_size.x / viewport_size.y);
                }
            }
    }
        // Render using editor camera
        const uint32_t scene_texture = renderer->get_scene_texture();

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
            ImGui::Image(reinterpret_cast<void*>(static_cast<uintptr_t>(scene_texture)), 
                        viewport_size, ImVec2(0, 1), ImVec2(1, 0));
        }
    }


    void ViewportComponent::render() {
        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImVec2 default_size = ImVec2(main_viewport->Size.x / 1.5f, main_viewport->Size.y / 1.5f);
        ImVec2 default_pos = ImVec2(
            main_viewport->Pos.x + (main_viewport->Size.x - default_size.x) * 0.5f,
            main_viewport->Pos.y + (main_viewport->Size.y - default_size.y) * 0.5f
        );
    
        ImGui::SetNextWindowSize(default_size, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(default_pos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(320, 180), ImVec2(UINT_MAX, UINT_MAX));
    
        const std::string window_name = context_->active_scene 
            ? context_->active_scene->get_name() 
            : "Viewport";
        
        if (ImGui::Begin(window_name.c_str())) {
            // Store the viewport bound for outside usage
            viewport_pos_ = ImGui::GetWindowPos();
            viewport_size_ = ImGui::GetWindowSize();
            viewport_hovered_ = ImGui::IsWindowHovered();
            last_mouse_pos_ = ImGui::GetMousePos();
            
            render_viewport_image();
            update_camera_control();
            render_viewport_stats_overlay();
        }
        ImGui::End();
            
    }


    void ViewportComponent::render_viewport_stats_overlay() {
        ImVec2 window_pos = ImGui::GetWindowPos();

        // Position overlay in top-left corner with padding
        const float padding = 10.0f;
        ImGui::SetNextWindowPos(ImVec2(window_pos.x + padding, window_pos.y + padding + ImGui::GetFrameHeight()));
        ImGui::SetNextWindowBgAlpha(0.35f);

        ImGuiWindowFlags overlay_flags =
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoFocusOnAppearing |
                ImGuiWindowFlags_NoNav |
                ImGuiWindowFlags_NoMove;

        if (ImGui::Begin("##ViewportStats", nullptr, overlay_flags)) {
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("Frame Time: %.2f ms", 1000.0f / ImGui::GetIO().Framerate);
            
            if (camera_active_) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
                ImGui::Text("Camera Active");
                ImGui::PopStyleColor();
            }

            if (context_->active_scene) {
                ImGui::Separator();
                ImGui::Text("Entities: %zu", context_->active_scene->get_entity_count());
            }

            if (editor_camera_) {
                ImGui::Separator();
                glm::vec3 pos = editor_camera_->transform()->get_position();
                ImGui::Text("Cam Pos: %.1f, %.1f, %.1f", pos.x, pos.y, pos.z);
            }
        }
        ImGui::End();
    }
}
