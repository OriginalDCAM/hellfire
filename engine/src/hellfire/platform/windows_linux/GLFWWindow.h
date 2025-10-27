//
// Created by denzel on 18/09/2025.
//

#pragma once
#include "../IWindow.h"
#include "GLFW/glfw3.h"

namespace hellfire {
    class GLFWWindow final : public IWindow {
    public:
        bool create(int width, int height, const std::string &title) override;

        void destroy() override;

        void swap_buffers() override;

        void poll_events() override;

        void wait_for_events() override;

        void set_title(const std::string &title) override;

        void set_size(int width, int height) override;

        glm::ivec2 get_size() const override;

        bool should_close() const override;

        void *get_native_handle() override;

        glm::ivec2 get_framebuffer_size() const override;

        bool is_key_pressed(int keycode) const override;

        glm::vec2 get_mouse_position() const override;

        void make_current() override;

    private:
        GLFWwindow *window_ = nullptr;
        IWindowEventHandler *event_handler_ = nullptr;
        bool initialized_ = false;

    private:
        // Static callbacks for GLFW
        static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

        static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

        static void cursor_position_callback(GLFWwindow *window, double x, double y);

        static void window_size_callback(GLFWwindow *window, int width, int height);

        static void window_iconifiy_callback(GLFWwindow *window, int iconified);


    public:
        void set_event_handler(IWindowEventHandler *handler) override;

        float get_elapsed_time() override;

        void warp_cursor(double x, double y) override;

        void set_cursor_mode(CursorMode mode) override;

        void enable_vsync(bool vsync) override;
    };
}
