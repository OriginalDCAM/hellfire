//
// Created by denzel on 05/12/2025.
//

#pragma once
#include "core/ApplicationState.h"
#include "hellfire/core/Application.h"
#include "ui/PanelManager.h"
#include "ui/Panels/MenuBar/MenuBarComponent.h"
#include "ui/Panels/Viewport/ViewportPanel.h"

namespace hellfire::editor {
    class EditorState : public ApplicationState {
    public:
        bool load_last_settings();

        void on_enter() override;

        void save_application_state();

        void on_exit() override;
        void render() override;

    private:
        void create_dockspace();

    public:
        bool on_mouse_move(float x, float y, float x_offset, float y_offset) override;

        bool on_mouse_wheel(float delta) override;

        Entity * get_render_camera_override() override;
        
        std::filesystem::path get_editor_settings_path() {
            return EditorConfig::get_config_path() / "editor_settings.json";
        }

        bool on_window_resize(int width, int height) override;

    private:
        PanelManager panel_manager_;
        ViewportPanel* viewport_panel_ = nullptr; // Raw pointer for quick access
        
        AppInfo editor_settings_ = {};

        // UI Components
        std::unique_ptr<MenuBarComponent> menu_bar_;
    };  
}
