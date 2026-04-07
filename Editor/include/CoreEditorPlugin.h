//
// Created by denzel on 23/09/2025.
//

#pragma once
#include "hellfire/Interfaces/IApplicationPlugin.h"
#include "../../Engine/src/hellfire/platform/IWindow.h"

namespace hellfire::editor {
    class CoreEditorPlugin final : public IApplicationPlugin {
    public:
        void on_initialize(Application &app) override;

        void initialize_imgui(IWindow *window);

        ~CoreEditorPlugin() override;

        void cleanup_imgui();

        void on_begin_frame() override;

        void on_end_frame() override;

        void on_render() override;

        void render_test_windows();

        void create_dockspace();

        bool on_key_down(int key) override;

        bool on_key_up(int key) override;

        bool on_mouse_button(int button, bool pressed) override;

        bool on_mouse_move(float x, float y) override;

        bool on_mouse_wheel(float delta) override;

        void on_window_resize(int width, int height) override;

        void on_window_focus(bool focused) override;
    private:
        Application* app_ = nullptr;
        bool imgui_initialized_ = false;
        bool show_demo_ = true;
    };
}
