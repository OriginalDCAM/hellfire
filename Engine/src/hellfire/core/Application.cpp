#include "Application.h"

#include "hellfire/utilities/ServiceLocator.h"
#include "../platform/windows_linux/GLFWWindow.h"

namespace hellfire {
    Application::Application(int width, int height, std::string title) : shader_registry_(&shader_manager_) {
        window_info_.width = width;
        window_info_.height = height;
        window_info_.aspect_ratio = static_cast<float>(width) / height;
    }

    Application::~Application() {
    }

    Shader *Application::ensure_fallback_shader() {
        try {
            Shader *shader = shader_registry_.load_and_get_shader(
                "assets/shaders/standard.vert",
                "assets/shaders/lambert.frag"
            );

            if (shader && shader->is_valid()) {
                return shader;
            }
        } catch (const std::exception &e) {
            std::cerr << "Warning: Could not load default shaders: " << e.what() << std::endl;
        }

        std::cerr << "Using minimal fallback shader" << std::endl;

        // Try to create a basic hardcoded shader as fallback
        uint32_t fallback_id = create_minimal_fallback_shader();
        if (fallback_id != 0) {
            return shader_registry_.get_shader_from_id(fallback_id);
        }

        return nullptr;
    }

    uint32_t Application::create_minimal_fallback_shader() {
        const char *vertex_source = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 MVP;
        void main() {
            gl_Position = MVP * vec4(aPos, 1.0);
        }
    )";

        const char *fragment_source = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 0.0, 1.0, 1.0); // Magenta to indicate fallback
        }
    )";

        try {
            return shader_manager_.compile_shader_program(vertex_source, fragment_source);
        } catch (const std::exception &e) {
            std::cerr << "Failed to create minimal fallback shader: " << e.what() << std::endl;
            return 0;
        }
    }


    void Application::initialize() {
        input_manager_ = std::make_unique<InputManager>();

        window_ = std::make_unique<GLFWWindow>();
        if (!window_->create(window_info_.width, window_info_.height, "Hellfire Engine")) {
            throw std::runtime_error("Failed to create window");
        }

        window_->set_event_handler(this);

        window_->make_current();
        if (glewInit() != GLEW_OK) {
            throw std::runtime_error("Failed to initialize GLEW");
        }

        // Register services
        ServiceLocator::register_service<Renderer>(&renderer_);
        ServiceLocator::register_service<InputManager>(input_manager_.get());
        ServiceLocator::register_service<ShaderManager>(&shader_manager_);
        ServiceLocator::register_service<IWindow>(window_.get());
        ServiceLocator::register_service<SceneManager>(&scene_manager_);

        // Initialize engine systems
        renderer_.init();

        // Create fallback shader
        Shader *fallback = ensure_fallback_shader();
        renderer_.set_fallback_shader(*fallback);

        call_plugins([this](IApplicationPlugin &plugin) {
            plugin.on_initialize(*this);
        });
    }

    void Application::run() {
        while (!window_->should_close()) {
            window_->poll_events();
            update_delta_time();

            input_manager_->update();

            // Update scene
            scene_manager_.update(delta_time_);

            on_render();
        }
    }


    void Application::on_render() {
        // Plugin begin_frame
        call_plugins([](IApplicationPlugin &plugin) {
            plugin.on_begin_frame();
        });
        renderer_.begin_frame();

        if (auto *active_scene = scene_manager_.get_active_scene()) {
            renderer_.render(*active_scene);
        }

        // Plugin render
        call_plugins([](IApplicationPlugin &plugin) {
            plugin.on_render();
        });

        renderer_.end_frame();

        // Plugin end_frame
        call_plugins([](IApplicationPlugin &plugin) {
            plugin.on_end_frame();
        });
        window_->swap_buffers();
    }

    void Application::on_key_down(int key) {
        bool consumed = call_plugins_until_consumed([key](IApplicationPlugin &plugin) {
            return plugin.on_key_down(key);
        });

        if (!consumed) {
            input_manager_->on_key_down(key);
        }
    }

    void Application::on_key_up(int key) {
        input_manager_->on_key_up(key);
    }

    void Application::on_mouse_button(int button, bool pressed) {
        bool consumed = call_plugins_until_consumed([button, pressed](IApplicationPlugin &plugin) {
            return plugin.on_mouse_button(button, pressed);
        });

        if (!consumed) {
            IWindowEventHandler::on_mouse_button(button, pressed);
        }
    }

    bool Application::handle_first_mouse_movement(float x, float y) {
        if (window_info_.first_mouse) {
            window_info_.mouse_pos.x = x;
            window_info_.mouse_pos.y = y;
            window_info_.first_mouse = false;
            return true;
        }
        return false;
    }

    void Application::handle_cursor_warping(float x, float y) const {
        if (window_info_.should_warp_cursor) {
            if (x < 100 || x > static_cast<float>(window_info_.width - 100) || y < 100 || y > static_cast<float>(
                    window_info_.height - 100)) {
                window_->warp_cursor(static_cast<double>(window_info_.width) / 2,
                                     static_cast<double>(window_info_.height) / 2);
            }
        }
    }

    void Application::on_mouse_move(float x, float y) {
        input_manager_->on_mouse_move(x, y);

        if (handle_first_mouse_movement(x, y)) return;

        const float x_offset = x - window_info_.mouse_pos.x;
        const float y_offset = window_info_.mouse_pos.y - y;

        window_info_.mouse_pos.x = x;
        window_info_.mouse_pos.y = y;
        
        // Skip large jumps (window focus, etc)
        if (abs(x_offset) > 100 || abs(y_offset) > 100) return;

        bool consumed = call_plugins_until_consumed([x, y, x_offset, y_offset](IApplicationPlugin &plugin) {
            return plugin.on_mouse_move(x, y, x_offset, y_offset);
        });

        if (consumed) return;
        
        // Warp when getting close to edges
        handle_cursor_warping(x, y);


    }

    void Application::on_window_resize(const int width, const int height) {
        window_info_.width = width;
        window_info_.height = height;
        window_info_.aspect_ratio = static_cast<float>(width) / static_cast<float>(height);

        glViewport(0, 0, width, height);

        // Update cameras
        for (const auto &camera: scene_manager_.get_camera_entities()) {
            camera->get_component<CameraComponent>()->set_aspect_ratio(window_info_.aspect_ratio);
        }

        call_plugins([width, height](IApplicationPlugin &plugin) {
            plugin.on_window_resize(width, height);
        });
    }

    void Application::update_delta_time() {
        const float current_time = window_->get_elapsed_time();
        delta_time_ = current_time - last_frame_time_;
        last_frame_time_ = current_time;
    }


}
