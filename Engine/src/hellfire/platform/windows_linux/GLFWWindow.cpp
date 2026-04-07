//
// Created by denzel on 18/09/2025.
//
#include "GLFWWindow.h"

namespace hellfire {
    bool GLFWWindow::create(int width, int height, const std::string &title) {
        if (!initialized_) {
            if (!glfwInit()) {
                return false;
            }
            initialized_ = true;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!window_) {
            return false;
        }

        glfwMakeContextCurrent(window_);
        glfwSetWindowUserPointer(window_, this);

        

        // Set callbacks
        glfwSetKeyCallback(window_, key_callback);
        glfwSetMouseButtonCallback(window_, mouse_button_callback);
        glfwSetCursorPosCallback(window_, cursor_position_callback);
        glfwSetWindowSizeCallback(window_, window_size_callback);
        glfwSetWindowIconifyCallback(window_, window_iconifiy_callback);

        return true;
    }

    void GLFWWindow::destroy() {
        if (window_) {
            glfwDestroyWindow(window_);
            window_ = nullptr;
        }

        if (initialized_) {
            glfwTerminate();
            initialized_ = false;
        }
    }

    void GLFWWindow::swap_buffers() {
        if (window_) {
            glfwSwapBuffers(window_);
        }
    }

    void GLFWWindow::poll_events() {
        if (window_) {
            glfwPollEvents();
        }
    }

    void GLFWWindow::wait_for_events() {
        glfwWaitEvents();
    }

    void GLFWWindow::set_title(const std::string &title) {
        window_info_.title = title;
        if (window_) {
            glfwSetWindowTitle(window_, title.c_str());
        }
    }

    void GLFWWindow::set_size(int width, int height) {
        window_info_.width = width;
        window_info_.height = height;
        if (window_) {
            glfwSetWindowSize(window_, width, height);
        }
    }

    glm::ivec2 GLFWWindow::get_size() const {
        return {window_info_.width, window_info_.height};
    }

    bool GLFWWindow::should_close() const {
        return window_ && glfwWindowShouldClose(window_);
    }

    void *GLFWWindow::get_native_handle() {
        return window_;
    }

    glm::ivec2 GLFWWindow::get_framebuffer_size() const {
        if (window_) {
            int width, height;
            glfwGetFramebufferSize(window_, &width, &height);
            return {width, height};
        }
        return {0, 0};
    }

    bool GLFWWindow::is_key_pressed(int keycode) const {
        if (window_) {
            return glfwGetKey(window_, keycode) == GLFW_PRESS;
        }
        return false;
    }

    glm::vec2 GLFWWindow::get_mouse_position() const {
        if (window_) {
            double x, y;
            glfwGetCursorPos(window_, &x, &y);
            return {static_cast<float>(x), static_cast<float>(y)};
        }
    }

    void GLFWWindow::make_current() {
        if (window_) {
            glfwMakeContextCurrent(window_);
        }
    }

    void GLFWWindow::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (const auto *instance = static_cast<GLFWWindow *>(glfwGetWindowUserPointer(window));
            instance && instance->event_handler_) {
            if (action == GLFW_PRESS) {
                instance->event_handler_->on_key_down(key);
            } else if (action == GLFW_RELEASE) {
                instance->event_handler_->on_key_up(key);
            }
        }
    }

    void GLFWWindow::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
        if (const auto *instance = static_cast<GLFWWindow *>(glfwGetWindowUserPointer(window));
            instance && instance->event_handler_) {
            if (action == GLFW_PRESS) {
                instance->event_handler_->on_mouse_button(button, true);
            } else if (action == GLFW_RELEASE) {
                instance->event_handler_->on_mouse_button(button, false);
            }
        }
    }

    void GLFWWindow::cursor_position_callback(GLFWwindow *window, double x, double y) {
        if (const auto *instance = static_cast<GLFWWindow *>(glfwGetWindowUserPointer(window));
            instance && instance->event_handler_) {
            instance->event_handler_->on_mouse_move(static_cast<float>(x), static_cast<float>(y));
        }
    }

    void GLFWWindow::window_size_callback(GLFWwindow *window, int width, int height) {
        if (auto *instance = static_cast<GLFWWindow *>(glfwGetWindowUserPointer(window))) {
            instance->window_info_.width = width;
            instance->window_info_.height = height;

            if (instance->event_handler_) {
                instance->event_handler_->on_window_resize(width, height);
            }
        }
    }

    void GLFWWindow::window_iconifiy_callback(GLFWwindow *window, const int iconified) {
        if (const auto *instance = static_cast<GLFWWindow *>(glfwGetWindowUserPointer(window))) {
            if (instance->event_handler_) {
                const bool minimized = iconified == GLFW_TRUE;
                instance->event_handler_->on_window_minimize(minimized);
            }
        }
    }

    void GLFWWindow::set_event_handler(IWindowEventHandler *handler) {
        event_handler_ = handler;
    }

    float GLFWWindow::get_elapsed_time() {
        return glfwGetTime();
    }

    void GLFWWindow::warp_cursor(double x, double y) {
        glfwSetCursorPos(window_, x, y);
    }

    void GLFWWindow::set_cursor_mode(CursorMode mode) {
        switch (mode) {
            case HIDDEN:
                glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                break;
            case DISABLED:
                glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                break;
            case SHOWN:
                glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;
        }
    }
}
