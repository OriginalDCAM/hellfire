//
// Created by denzel on 23/09/2025.
//

#pragma once

#include "StateManager.h"
#include "../ui/Panels/EditorPanel.h"
#include "hellfire/Interfaces/IApplicationPlugin.h"
#include "hellfire/platform/IWindow.h"
#include "../ui/Panels/Inspector/InspectorPanel.h"

namespace hellfire::editor {
    class EditorApplication : public IApplicationPlugin {
    public:
        void on_initialize(Application &app) override;

        void initialize_imgui(IWindow *window);

        ~EditorApplication() override;

        void cleanup_imgui();

        void on_begin_frame() override;

        void on_end_frame() override;

        void sync_editor_context();

        void on_render() override;

        bool on_key_down(int key) override;

        bool on_key_up(int key) override;

        bool on_mouse_button(int button, bool pressed) override;

        bool on_mouse_move(float x, float y, float x_offset, float y_offset) override;

        bool on_mouse_wheel(float delta) override;

        void on_window_resize(int width, int height) override;

        void on_window_focus(bool focused) override;

        Entity *get_render_camera_override() override;

    protected:
        EditorContext editor_context_;

    private:
        bool imgui_initialized_ = false;

        StateManager state_manager_;
    };
}
