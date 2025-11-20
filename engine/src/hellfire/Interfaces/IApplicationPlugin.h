//
// Created by denzel on 23/09/2025.
//

#pragma once

namespace hellfire {
    class Application;

    class IApplicationPlugin {
    public:
        virtual ~IApplicationPlugin() = default;

        // Lifecycle events
        virtual void on_initialize(Application &app) {
        }

        virtual void on_begin_frame() {
        }

        virtual void on_end_frame() {
        }

        virtual void on_render() {
        }

        // Input events - return true if consumed
        virtual bool on_key_down(int key) { return false; }
        virtual bool on_key_up(int key) { return false; }
        virtual bool on_mouse_button(int button, bool pressed) { return false; }
        virtual bool on_mouse_move(float x, float y, float x_offset, float y_offset) { return false; }
        virtual bool on_mouse_wheel(float delta) { return false; }

        // Window events
        virtual void on_window_resize(int width, int height) {
        }

        virtual void on_window_focus(bool focused) {
        }

        // Allow plugins to override camera
        virtual Entity* get_render_camera_override() { return nullptr; }
    };
}
