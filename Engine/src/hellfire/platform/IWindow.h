//
// Created by denzel on 18/09/2025.
//

#pragma once
#include <string>

#include "glm/vec2.hpp"

namespace hellfire {
    enum CursorMode {
        HIDDEN,
        DISABLED,
        SHOWN
    };

    struct WindowInfo {
        int width;
        int height;
        std::string title;
    };

    class IWindowEventHandler {
    public:
        virtual ~IWindowEventHandler() = default;

        // @brief Generic input events - no platform-specific parameters
        virtual void on_key_down(int keycode) {
        }

        virtual void on_key_up(int keycode) {
        }

        virtual void on_mouse_button(int button, bool pressed) {
        }

        virtual void on_mouse_move(float x, float y) {
        }

        virtual void on_mouse_wheel(float delta) {
        }

        virtual void on_window_resize(int width, int height) {
        }

        virtual void on_window_minimize(bool minimized) {
        }

        virtual void on_render() {
        }
    };

    class IWindow {
    public:
        virtual ~IWindow() = default;

        // Core window operations
        virtual bool create(int width, int height, const std::string &title) = 0;

        virtual void destroy() = 0;

        virtual void swap_buffers() = 0;

        virtual void poll_events() = 0;

        virtual void wait_for_events() = 0;

        virtual void set_event_handler(IWindowEventHandler *event_handler) = 0;

        // Window properties
        virtual void set_title(const std::string &title) = 0;

        virtual void set_size(int width, int height) = 0;

        virtual glm::ivec2 get_size() const = 0;

        virtual glm::ivec2 get_framebuffer_size() const = 0;


        // Input handling
        virtual bool is_key_pressed(int keycode) const = 0;

        virtual glm::vec2 get_mouse_position() const = 0;

        virtual bool should_close() const = 0;

        // Cursor properties
        virtual void set_cursor_mode(CursorMode mode) = 0;

        // OpenGL context
        virtual void make_current() = 0;

        virtual void *get_native_handle() = 0;

        virtual float get_elapsed_time() = 0;

        virtual void warp_cursor(double x, double y) = 0;

    protected:
        WindowInfo window_info_ = {};
    };
}
