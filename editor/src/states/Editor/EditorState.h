//
// Created by denzel on 05/12/2025.
//

#pragma once
#include "core/ApplicationState.h"
#include "ui/PanelManager.h"
#include "ui/Panels/MenuBar/MenuBarComponent.h"
#include "ui/Panels/Viewport/ViewportPanel.h"

namespace hellfire::editor {
    class EditorState : public ApplicationState {
    public:
        void on_enter() override;
        void on_exit() override;
        void render() override;

    private:
        void create_dockspace();

    public:
        bool on_mouse_move(float x, float y, float x_offset, float y_offset) override;

        bool on_mouse_wheel(float delta) override;

        Entity * get_render_camera_override() override;

    private:
        PanelManager panel_manager_;
        ViewportPanel* viewport_panel_ = nullptr; // Raw pointer for quick access

        // UI Components
        std::unique_ptr<MenuBarComponent> menu_bar_;
    };  
}
