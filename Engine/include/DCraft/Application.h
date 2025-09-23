// Application.h
#pragma once
#include "IWindow.h"
#include "Addons/SceneManager.h"
#include "Graphics/Renderer.h"
#include "Graphics/Managers/ShaderManager.h"
#include "DCraft/Interfaces/IApplicationPlugin.h"
#include "Managers/InputManager.h"

namespace hellfire {
    class Renderer;
    class Camera;

    struct MousePos {
        float x;
        float y;
    };

    struct AppInfo {
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
        bool first_mouse = true;

        MousePos mouse_pos;
        bool should_warp_cursor = true;
    };

    class Application;

    struct ApplicationCallbacks {
        std::function<void(Application &)> init = nullptr;
        std::function<void(SceneManager &, const AppInfo &, ShaderManager &)> setup = nullptr;
        std::function<void(float)> update = nullptr;
        std::function<void()> render = nullptr;
        std::function<void(Application &, float)> process_input = nullptr;
        std::function<void(float, float)> on_mouse_moved = nullptr;
    };

    class Application : public IWindowEventHandler {
    public:
        explicit Application(int width = 800, int height = 600, std::string title = "hellfire Application");
        ~Application() override;

        Shader *ensure_fallback_shader();
        uint32_t create_minimal_fallback_shader();

        // Clean initialization
        void initialize();
        void run();

        // Accessors
        [[nodiscard]] AppInfo &get_window_info() { return window_info_; }

        [[nodiscard]] bool is_key_pressed(int keycode) const;

        [[nodiscard]] float get_delta_time() const { return delta_time_; }
        ShaderManager &get_shader_manager() { return shader_manager_; }
        Renderer &get_renderer() { return renderer_; }

        // Callbacks
        void set_callbacks(const ApplicationCallbacks &callbacks) { callbacks_ = callbacks; }

        /// Method for registering plugins
        void register_plugin(std::unique_ptr<IApplicationPlugin> plugin) {
            plugins_.push_back(std::move(plugin));
        }

    protected:
        // IWindowEventHandler implementation
        void on_render() override;

        void on_key_down(int key) override;

        void on_key_up(int key) override;

        void on_mouse_button(int button, bool pressed) override;

        bool handle_first_mouse_movement(float x, float y);

        void handle_cursor_warping(float x, float y) const;

        void on_mouse_move(float x, float y) override;

        void on_window_resize(int width, int height) override;
    private:
        // Managers
        std::vector<std::unique_ptr<IApplicationPlugin>> plugins_;
        std::unique_ptr<IWindow> window_;
        std::unique_ptr<InputManager> input_manager_;

        ShaderManager shader_manager_;
        ShaderRegistry shader_registry_;
        SceneManager scene_manager_;
        Renderer renderer_;

        // Window info tracking
        AppInfo window_info_;

        // Input state 
        bool keys_[512] = {false};
        bool keys_just_pressed_[512] = {false};
        bool keys_just_released_[512] = {false};

        // Timing
        float last_frame_time_ = 0.0f;
        float delta_time_ = 0.0f;

        // Callbacks 
        ApplicationCallbacks callbacks_;

        void update_delta_time();


        template<typename Func>
        void call_plugins(Func&& func) {
            for (auto& plugin : plugins_) {
                func(*plugin);
            }
        }

        template<typename Func>
        bool call_plugins_until_consumed(Func&& func) {
            for (auto& plugin : plugins_) {
                if (func(*plugin)) return true;
            }
            return false;
        }
    };
}
