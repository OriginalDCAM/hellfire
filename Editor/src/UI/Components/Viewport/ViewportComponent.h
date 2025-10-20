//
// Created by denzel on 13/10/2025.
//

#pragma once
#include <imgui.h>

#include "UI/Components/EditorComponent.h"

namespace hellfire::editor {
    
class ViewportComponent final : public EditorComponent {
public:
    ViewportComponent();
    ~ViewportComponent() override;

    void render() override;
    void render_viewport_stats_overlay() const;

    Entity* get_editor_camera() const { return editor_camera_; }
    bool is_editor_camera_active() const { return camera_active_; }

    ImVec2 get_viewport_pos() { return viewport_pos_; }

    ImVec2 get_viewport_size() { return viewport_size_; }
    bool is_viewport_hovered() const { return viewport_hovered_; }
private:
    void create_editor_camera();
    void destroy_editor_camera();
    void update_camera_control();
    void render_viewport_image();

    void render_transform_gizmo() const;

    Entity* editor_camera_ = nullptr;
    bool camera_active_ = false;

    float last_resize_time_ = 0.0f;

    // Viewport position
    ImVec2 viewport_pos_;
    // The size of the viewport
    ImVec2 viewport_size_;
    // Whether the viewport is hovered
    bool viewport_hovered_ = false;
    ImVec2 last_mouse_pos_;
};
    
}
