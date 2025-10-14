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
    void render_viewport_stats_overlay();

    Entity* get_editor_camera() const { return editor_camera_; }
    bool is_editor_camera_active() const { return camera_active_; }
private:
    void create_editor_camera();
    void destroy_editor_camera();
    void update_camera_control();
    void render_viewport_image();
    
    Entity* editor_camera_ = nullptr;
    bool camera_active_ = false;

    ImVec2 last_viewport_size_ = {800,600};
    float last_resize_time_ = 0.0f;
};
    
}
