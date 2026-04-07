#pragma once
#include <functional>
#include <string>

#include "DCraft/Addons/SceneManager.h"
#include "DCraft/Graphics/Renderer.h"
#include "DCraft/Addons/PerspectiveCamera.h"

// READ: The application class relies on the freeGLUT api to handle windows, callbacks.
// freeGLUT is an ancient api that shouldn't be used in modern days. But had to use this

// TODO: Abstract the window into its own class, so that it could be used with multiple windowing apis like win32, glfw, sdl, etc.

namespace DCraft {
    struct WindowInfo {
        // Dimensions
        int width;
        int height;
        float aspect_ratio;

        // state
        bool is_fullscreen;
        bool is_resizable;

        // Input
        bool cursor_visible;
        bool cursor_locked;

        float frame_time;
        int fps;
    };

    class Application;

    struct ApplicationCallbacks {
        std::function<void(Application &)> init = nullptr;
        std::function<void(SceneManager &, const WindowInfo &)> setup = nullptr;
        std::function<void(float)> update = nullptr;
        std::function<void()> render = nullptr;
        std::function<void(Application &, float)> process_input = nullptr;
        std::function<void(float, float)> on_mouse_moved = nullptr;
    };

    class Application {
    public:
        Application(int width = 800, int height = 600, const std::string &title = "OpenGL Application");

        ~Application();


        int get_window_width() { return window_info_.width; }
        int get_window_height() { return window_info_.height; }

        uint32_t create_shader_program(const std::string &vertex_path, const std::string &fragment_path);

        void configure_camera(float fov, float near_plane, float far_plane, glm::vec3 position);

        Application(const Application &) = delete;

        Application &operator=(const Application &) = delete;

        bool is_ready;

        void load_scene();

        // Main application methods
        void initialize(int argc, char **argv);

        void run();

        void update();

        void render_frame() const;

        /// public method function: on_key_down
        /// @param key ASCII table keycode
        /// @param x [Discarded]
        /// @param y [Discarded]
        void on_key_down(unsigned char key);

        /// public method function:on_key_up 
        /// @param key ASCII table keycode
        /// @param x [Discarded]
        /// @param y [Discarded]
        void on_key_up(unsigned char key);

        /// public method function:on_special_key_down
        /// handles special keys down events
        /// @param key ASCII table keycode
        /// @param x [Discarded]
        /// @param y [Discarded]
        void on_special_key_down(int key);

        /// public method function:on_special_key_up
        /// handles special keys up events
        /// @param key ASCII table keycode
        /// @param x [Discarded]
        /// @param y [Discarded]
        void on_special_key_up(int key);

        // TODO: write JavaDOC comments
        void on_mouse_button(int button, int state, int x, int y);

        // TODO: write JavaDOC comments
        void on_mouse_motion(int x, int y);

        // TODO: write JavaDOC comments
        void on_mouse_passive_motion(int x, int y);

        // TODO: write JavaDOC comments
        void on_mouse_wheel(int wheel, int direction, int x, int y);

        // TODO: write JavaDOC comments
        void on_window_resize(int width, int height);

        // Game state methods
        // void load_scene();

        void process_input();

        void toggle_fullscreen();

        // Accessor methods
        const WindowInfo &get_window_info() const { return window_info_; }
        bool is_key_pressed(unsigned char key) const { return keys_[key]; }
        // Add the 256 index offset to special keys
        bool is_special_key_pressed(unsigned char key) const { return keys_[key + 256]; }
        float get_delta_time() const { return delta_time_; }
        void set_callbacks(const ApplicationCallbacks &callbacks) { callbacks_ = callbacks; }
        void set_shader_program(uint32_t shader_program_id) { shader_program_id_ = shader_program_id; }

        static Application &get_instance() { return *instance_; }

    protected:
        SceneManager scene_manager_;
        PerspectiveCamera *camera_ = nullptr;
        Renderer *renderer_ = nullptr;

    private:
        static Application *instance_;
        ApplicationCallbacks callbacks_;
        WindowInfo window_info_;
        std::string title_;
        uint32_t shader_program_id_;

        // Keys stuff
        bool keys_[512] = {false};
        // Window stuff
        bool is_fullscreen_ = false;
        bool warping_ = false;

        // Programs
        std::vector<Camera *> cameras_;
        std::vector<uint32_t> shader_programs_;

        // Mouse things
        int last_mouse_x_ = window_info_.width / 2;
        int last_mouse_y_ = window_info_.height / 2;
        bool first_mouse_ = true;

        // Timing
        float last_frame_time_ = 0.0f;
        float delta_time_ = 0.0f;

        // Setup callbacks method
        void setup_callbacks();

        // Static callback methods for GLUT
        static void display_callback() {
            if (instance_) instance_->render_frame();
        }

        static void timer_callback(int value) {
            if (instance_) {
                instance_->update();
                glutTimerFunc(1, timer_callback, 0);
            }
        }

        static void window_resize_callback(int width, int height) {
            if (instance_) instance_->on_window_resize(width, height);
        }

        static void key_down_callback(unsigned char key, int x, int y) {
            if (instance_) instance_->on_key_down(key);
        }

        static void key_up_callback(unsigned char key, int x, int y) {
            if (instance_) instance_->on_key_up(key);
        }

        static void special_key_down_callback(int key, int x, int y) {
            if (instance_) instance_->on_special_key_down(key);
        }

        static void special_key_up_callback(int key, int x, int y) {
            if (instance_) instance_->on_special_key_up(key);
        }

        static void mouse_button_callback(int button, int state, int x, int y) {
            if (instance_) instance_->on_mouse_button(button, state, x, y);
        }

        static void mouse_motion_callback(int x, int y) {
            if (instance_) instance_->on_mouse_motion(x, y);
        }

        static void mouse_passive_motion_callback(int x, int y) {
            if (instance_) instance_->on_mouse_passive_motion(x, y);
        }

        static void mouse_wheel_callback(int wheel, int direction, int x, int y) {
            if (instance_) instance_->on_mouse_wheel(wheel, direction, x, y);
        }
    };
}
