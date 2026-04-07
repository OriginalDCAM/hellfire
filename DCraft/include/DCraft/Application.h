#pragma once
#include <cstdint>
#include <functional>
#include <imgui_impl_glut.h>
#include <string>

#include "Addons/SceneManager.h"
// #include "Editor/SceneEditorOverlay.h"
#include "Graphics/Renderer.h"
#include "Graphics/Managers/ShaderManager.hpp"


namespace DCraft {
    class Renderer;
    class Camera;
    
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
        std::function<void(SceneManager &, const WindowInfo &, ShaderManager&)> setup = nullptr;
        std::function<void(float)> update = nullptr;
        std::function<void()> render = nullptr;
        std::function<void(Application &, float)> process_input = nullptr;
        std::function<void(float, float)> on_mouse_moved = nullptr;
    };

    class Application {
    public:
        explicit Application(int width = 800, int height = 600, std::string title = "OpenGL Application");

        ~Application();


        [[nodiscard]] int get_window_width() const { return window_info_.width; }
        [[nodiscard]] int get_window_height() const { return window_info_.height; }

        Application(const Application &) = delete;

        Application &operator=(const Application &) = delete;

        bool is_ready;
        int ImGui_ImplGLUT_MouseButtonCallback;

        void load_scene();

        // Main application methods
        void initialize(int argc, char **argv);

        void run();

        void update();

        void render_frame();

        /// public method function: on_key_down
        /// @param key ASCII table keycode
        void on_key_down(unsigned char key);

        /// public method function:on_key_up 
        /// @param key ASCII table keycode
        void on_key_up(unsigned char key);

        /// public method function:on_special_key_down
        /// handles special keys down events
        /// @param key ASCII table keycode
        void on_special_key_down(int key);

        /// public method function:on_special_key_up
        /// handles special keys up events
        /// @param key ASCII table keycode
        void on_special_key_up(int key);

        /// public method function: on_mouse_button
        /// handles mouse button presses
        /// @param button Which mouse button
        /// @param state Pressed or Released
        void on_mouse_button(int button, int state, int x, int y);

        /// public method function: on_mouse_motion
        /// handles mouse motion
        /// @param x offset
        /// @param y offset
        void on_mouse_motion(int x, int y);

        
        /// public method function: on_mouse_passive_motion
        /// handles passive mouse motion, just regular mouse motion without a holding mouse button
        /// @param x offset
        /// @param y offset
        void on_mouse_passive_motion(int x, int y);

        // TODO: write JavaDOC comments
        void on_mouse_wheel(int wheel, int direction, int x, int y);

        void on_window_resize(int width, int height);

        // Game state methods
        bool game_mode_changed = false;

        void process_input();
        void toggle_fullscreen();

        uint32_t ensure_fallback_shader();

        // Accessor methods
        [[nodiscard]] const WindowInfo &get_window_info() const { return window_info_; }
        [[nodiscard]] bool is_key_pressed(unsigned char key) const { return keys_[key]; }
        // Add the 256 index offset to special keys
        [[nodiscard]] bool is_special_key_pressed(unsigned char key) const { return keys_[key + 256]; }
        [[nodiscard]] float get_delta_time() const { return delta_time_; }
        void set_callbacks(const ApplicationCallbacks &callbacks) { callbacks_ = callbacks; }
        
        ShaderManager& get_shader_manager() { return shader_manager_; }

        void toggle_editor_mode();
        bool is_game_mode() const { return game_mode_; }
        bool is_editor_mode() const { return !game_mode_; }
        
        static Application &get_instance() { return *instance_; }
    protected:
        SceneManager scene_manager_;
        Renderer renderer_;
    private:
        static Application *instance_;
        ApplicationCallbacks callbacks_;
        WindowInfo window_info_;
        std::string title_;
        ShaderManager shader_manager_;

        std::array<bool, 512> active_keys_before_mode_change_; 
        Entity* selected_node_ = nullptr;
        // Editor::SceneEditorOverlay editor_overlay_;

        // Keys stuff
        bool editor_mode_;
        bool game_mode_;
        bool keys_[512] = {false};
        bool prev_keys_[512] = {false};
        // Window stuff
        bool is_fullscreen_ = false;
        bool warping_ = false;

        // Programs
        std::vector<Camera *> cameras_;

        // Mouse things
        int last_mouse_x_ = window_info_.width / 2;
        int last_mouse_y_ = window_info_.height / 2;
        int mouse_delta_x_;
        int mouse_delta_y_;
        bool first_mouse_ = true;

        // Timing
        float last_frame_time_ = 0.0f;
        float delta_time_ = 0.0f;

        // Setup callbacks method
        static void setup_callbacks();

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
            ImGui_ImplGLUT_ReshapeFunc(width, height); 
            if (instance_) instance_->on_window_resize(width, height);
        }

        static void key_down_callback(unsigned char key, int x, int y) {
            ImGui_ImplGLUT_KeyboardFunc(key, x, y);
            if (instance_) instance_->on_key_down(key);
        }

        static void key_up_callback(unsigned char key, int x, int y) {
            ImGui_ImplGLUT_KeyboardUpFunc(key, x, y);
            if (instance_) instance_->on_key_up(key);
        }

        static void special_key_down_callback(int key, int x, int y) {
            ImGui_ImplGLUT_SpecialFunc(key, x, y);
            if (instance_) instance_->on_special_key_down(key);
        }

        static void special_key_up_callback(int key, int x, int y) {
            ImGui_ImplGLUT_SpecialUpFunc(key, x, y);
            if (instance_) instance_->on_special_key_up(key);
        }

        static void mouse_button_callback(int button, int state, int x, int y) {
            ImGui_ImplGLUT_MouseFunc(button, state, x, y);
            if (instance_) instance_->on_mouse_button(button, state, x, y);
        }

        static void mouse_motion_callback(int x, int y) {
            ImGui_ImplGLUT_MotionFunc(x, y);
            if (instance_) instance_->on_mouse_motion(x, y);
        }

        static void mouse_passive_motion_callback(int x, int y) {
            ImGui_ImplGLUT_MotionFunc(x, y);  // ImGui uses the same handler for both motion types
            if (instance_) instance_->on_mouse_passive_motion(x, y);
        }

        static void mouse_wheel_callback(int wheel, int direction, int x, int y) {
            ImGui_ImplGLUT_MouseWheelFunc(wheel, direction, x, y);
            if (instance_) instance_->on_mouse_wheel(wheel, direction, x, y);
        }
    };
}
