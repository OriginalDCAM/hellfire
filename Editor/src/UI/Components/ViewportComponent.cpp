//
// Created by denzel on 13/10/2025.
//

#include "ViewportComponent.h"
#include "hellfire/graphics/renderer/Renderer.h"

#include "imgui.h"

namespace hellfire::editor {
    void TextCentered(const std::string &text) {
        auto viewport_size = ImGui::GetContentRegionAvail();
        auto text_width = ImGui::CalcTextSize(text.c_str()).x;
        ImGui::SetCursorPos(ImVec2((viewport_size.x - text_width) / 2, viewport_size.y / 2));
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text(text.c_str());
        ImGui::SetWindowFontScale(1.0f);
    }
    
    void ViewportComponent::render() {
         if (!context_->active_scene) return;

        const std::string &scene_name = context_->active_scene->get_name();

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImVec2 default_size = ImVec2(viewport->Size.x / 1.5, viewport->Size.y / 1.5);
        ImVec2 default_pos = ImVec2(
            viewport->Pos.x + (viewport->Size.x - default_size.x) * 0.5f,
            viewport->Pos.y + (viewport->Size.y - default_size.y) * 0.5f
        );

        ImGui::SetNextWindowSize(default_size, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(default_pos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(320, 180), ImVec2(UINT_MAX, UINT_MAX));

        if (ImGui::Begin(scene_name.c_str())) {
            ImVec2 viewport_size = ImGui::GetContentRegionAvail();

            // Store last valid size
            static ImVec2 last_valid_size = {800, 600};
            static float last_resize_time = 0.0f;
            constexpr float RESIZE_DELAY = 0.016f; 

            // Use last valid size if current one is invalid
            if (viewport_size.x > 0 && viewport_size.y > 0) {
                last_valid_size = viewport_size;
            } else {
                viewport_size = last_valid_size;
            }

            auto *renderer = ServiceLocator::get_service<Renderer>();
            const auto current_time = static_cast<float>(ImGui::GetTime());

            // Only resize if dimensions actually changed
            static ImVec2 last_viewport_size = {0, 0};
            if (viewport_size.x != last_viewport_size.x || viewport_size.y != last_viewport_size.y &&  (current_time - last_resize_time) > RESIZE_DELAY) {
                renderer->resize_scene_framebuffer(static_cast<uint32_t>(viewport_size.x),
                                                   static_cast<uint32_t>(viewport_size.y));
                last_viewport_size = viewport_size;
                last_resize_time = current_time;

                // Update camera's aspect ratio
                if (auto *camera = context_->active_scene->get_active_camera()) {
                    const float aspect_ratio = viewport_size.x / viewport_size.y;
                    camera->set_aspect_ratio(aspect_ratio);
                }
            }

            const uint32_t scene_texture = renderer->get_scene_texture();
            if (!context_->active_scene->get_active_camera()) {
                TextCentered("No Active Camera In Scene!");
            } else {
                ImGui::Image(scene_texture, viewport_size);
            }

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

            if (context_->active_scene) {
                ImGui::Separator();
                ImGui::Text("Entities: %zu", context_->active_scene->get_entity_count());
            }
        }
        ImGui::End();
    }


}
