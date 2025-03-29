#pragma once
#include <functional>
#include <string>

#include "Core/SceneManager.h"
#include "Core/Graphics/Renderer.h"
#include "Cameras/PerspectiveCamera.h"

// READ: The application class relies on the freeGLUT api to handle windows, callbacks.
// freeGLUT is an ancient api that shouldn't be used in modern days. But had to use this

// TODO: Abstract the window into its own class, so that it could be used with multiple windowing apis like win32, glfw, sdl, etc.

namespace DCraft
{
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

    struct ApplicationCallbacks  
    {
        std::function<void(Application&)> init = nullptr;
        std::function<void(SceneManager&, const WindowInfo&)> setup = nullptr;
        std::function<void(float)> update = nullptr;
        std::function<void()> render = nullptr;
    };

    class Application {
    public:
        /// <summary>
        /// public constructor: Application
        /// @param width: the width of the initial window
        /// @param height: the height of the initial window
        /// @param title: sets the title of the window
        /// </summary>
        Application(int width = 800, int height = 600, const std::string& title = "OpenGL Application");
        ~Application();
        int get_window_width() { return width_; }
        int get_window_height() { return height_; }
        uint32_t create_shader_program(const std::string& vertex_path, const std::string& fragment_path);
        void configure_camera(float fov, float near_plane, float far_plane, glm::vec3 position);

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        bool is_ready;

        // Main application methods
        void initialize(int argc, char** argv);
        void run();
        void update();
        void render_frame();

        /// public method function: on_key_down
        /// @param key ASCII table keycode
        /// @param x [Discarded]
        /// @param y [Discarded]
        void on_key_down(unsigned char key, int x, int y);

        /// public method function:on_key_up 
        /// @param key ASCII table keycode
        /// @param x [Discarded]
        /// @param y [Discarded]
        void on_key_up(unsigned char key, int x, int y);
        /// public method function:on_special_key_down
        /// handles special keys down events
        /// @param key ASCII table keycode
        /// @param x [Discarded]
        /// @param y [Discarded]
        void on_special_key_down(int key, int x, int y);
        /// public method function:on_special_key_up
        /// handles special keys up events
        /// @param key ASCII table keycode
        /// @param x [Discarded]
        /// @param y [Discarded]
        void on_special_key_up(int key, int x, int y);
        // TODO: write JavaDOC comments
        void on_mouse_button(int button, int state, int x, int y);
        // TODO: write JavaDOC comments
        void on_mouse_motion(int x, int y);
        // TODO: write JavaDOC comments
        void on_mouse_passive_motion(int x, int y);
        // TODO: write JavaDOC comments
        void on_mouse_wheel(int wheel, int direction, int x, int y);

        // Game state methods
        void load_scene();
        void process_input();

        // Accessor methods
        PerspectiveCamera* get_camera() const { return camera_; }
        bool is_key_pressed(unsigned char key) const { return keys_[key]; }
        float get_delta_time() const { return delta_time_; }
        void set_callbacks(const ApplicationCallbacks& callbacks) { callbacks_ = callbacks; }
        void set_shader_program(uint32_t shader_program_id) { shader_program_id_ = shader_program_id; }

    protected:
        SceneManager scene_manager_;
        PerspectiveCamera* camera_ = nullptr;
        Renderer* renderer_ = nullptr;
    private:
        static Application* instance_;
        ApplicationCallbacks callbacks_;
        int width_, height_;
        std::string title_;
        uint32_t shader_program_id_;

        // Programs
        std::vector<Camera*> cameras_;
        std::vector<uint32_t> shader_programs_;

        int last_mouse_x_ = width_ / 2;
        int last_mouse_y_ = height_ / 2;
        bool first_mouse_ = true;

        // Input state
        bool keys_[256] = { false };

        // Timing
        float last_frame_time_ = 0.0f;
        float delta_time_ = 0.0f;

        // Private methods
        void setup_callbacks();

        // Static callback methods for GLUT
        static void display_callback() {
            if (instance_) instance_->render_frame();
        }

        static void timer_callback(int value) {
            if (instance_) {
                instance_->update();
                glutTimerFunc(16, timer_callback, 0);
            }
        }

        static void key_down_callback(unsigned char key, int x, int y) {
            if (instance_) instance_->on_key_down(key, x, y);
        }

        static void key_up_callback(unsigned char key, int x, int y) {
            if (instance_) instance_->on_key_up(key, x, y);
        }

        static void special_key_down_callback(int key, int x, int y) {
            if (instance_) instance_->on_special_key_down(key, x, y);
        }

        static void special_key_up_callback(int key, int x, int y) {
            if (instance_) instance_->on_special_key_up(key, x, y);
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


