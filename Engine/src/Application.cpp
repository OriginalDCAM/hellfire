#include "DCraft/Application.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_std.h>
#include "imgui/backends/imgui_impl_glut.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/imgui.h"

#include "DCraft/Graphics/Renderer.h"
#include "DCraft/Addons/SceneManager.h"
#include "DCraft/Structs/Scene.h"

#include <iostream>
#include <utility>

#include "DCraft/Components/CameraComponent.h"

namespace DCraft {
    Application::Application(int width, int height, std::string title) : title_(std::move(title)),
                                                                         shader_registry_(&shader_manager_) {
        if (instance_ != nullptr) {
            throw std::runtime_error("Singleton Application already created");
        }

        // Set the structs properties 
        window_info_.width = width;
        window_info_.height = height;
        window_info_.is_fullscreen = false;
        window_info_.aspect_ratio = static_cast<float>(width) / static_cast<float>(height);

        instance_ = this;
    }

    Application::~Application() {
        // Clean up ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGLUT_Shutdown();
        ImGui::DestroyContext();

        // Cleanup application
        if (instance_ == this) {
            instance_ = nullptr;
        }

        std::clog << "Application destroyed" << std::endl;
    }

    void Application::initialize_glut(int argc, char **argv) {
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
        glutInitWindowSize(window_info_.width, window_info_.height);
        glutCreateWindow(title_.c_str());
        std::clog << "GLUT initialization succeeded" << '\n';
    }

    void Application::initialize_imgui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.IniFilename = "editorconfig.ini";
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        io.DisplaySize.x = static_cast<float>(window_info_.width);
        io.DisplaySize.y = static_cast<float>(window_info_.height);

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGLUT_Init();
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void Application::initialize(int argc, char **argv) {
        std::clog << "Initializing application..." << '\n';

        // Initialize GLUT
        initialize_glut(argc, argv);

        // Initialize GLEW
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << '\n';
            exit(1);
        }
        std::clog << "GLEW initialization succeeded" << '\n';

        // Setup GLUT callbacks
        setup_callbacks();
        std::clog << "GLUT callbacks setup succeeded" << '\n';

        // Initialize ImGui
        initialize_imgui();

        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_CONTINUE_EXECUTION);

        if (callbacks_.init) {
            callbacks_.init(*this);
        }

        Shader *fallback_shader = ensure_fallback_shader();
        renderer_.set_fallback_shader(*fallback_shader);
        renderer_.init();
        std::clog << "Renderer setup succeeded" << '\n';

        // Create initial framebuffer for editor
        renderer_.create_scene_framebuffer(800, 600);

        load_scene();

        std::clog << "Application initialization complete" << std::endl;
    }

    void Application::setup_callbacks() {
        glutDisplayFunc(display_callback);
        glutTimerFunc(8, timer_callback, 0);
        glutReshapeFunc(window_resize_callback);
        glutKeyboardFunc(key_down_callback);
        glutKeyboardUpFunc(key_up_callback);
        glutSpecialFunc(special_key_down_callback);
        glutSpecialUpFunc(special_key_up_callback);
        glutMouseFunc(mouse_button_callback);
        glutMotionFunc(mouse_motion_callback);
        glutPassiveMotionFunc(mouse_passive_motion_callback);
        glutMouseWheelFunc(mouse_wheel_callback);
    }

    void Application::run() {
        glutMainLoop();
    }

    void Application::update() {
        // Calculate delta time
        float current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
        delta_time_ = current_time - last_frame_time_;
        last_frame_time_ = current_time;

        process_input();

        scene_manager_.update(delta_time_);

        if (callbacks_.update) {
            callbacks_.update(delta_time_);
        }

        clear_frame_input_flags();

        glutPostRedisplay();
    }

    void Application::render_frame() {
        Scene *active_scene = scene_manager_.get_active_scene();
        if (!active_scene) {
            std::cerr << "No active scene to render!" << std::endl;
            return;
        }

        renderer_.set_render_to_framebuffer(false);

        renderer_.begin_frame();

        renderer_.render(*active_scene);

        renderer_.end_frame();

        glutSwapBuffers();
    }

    void Application::check_key_timeouts() {
        static const float KEY_TIMEOUT = 0.1f;

        for (int i = 0; i < 512; i++) {
            if (keys_[i]) {
                key_timers_[i] += delta_time_;

                if (key_timers_[i] > KEY_TIMEOUT) {
                    bool is_modifier = false;

                    if (i >= 256) {
                        int special_key = i - 256;
                        is_modifier = (special_key >= GLUT_KEY_SHIFT_L && special_key <= GLUT_KEY_SHIFT_R) ||
                                      (special_key >= GLUT_KEY_CTRL_L && special_key <= GLUT_KEY_CTRL_R) ||
                                      (special_key >= GLUT_KEY_ALT_L && special_key <= GLUT_KEY_ALT_R);
                    } else {
                        is_modifier = (i < 32 && i != 27);
                    }

                    if (is_modifier) {
                        std::cout << "Warning: Forcing release of stuck modifier key " << i << std::endl;
                        keys_just_released_[i] = true;
                        keys_[i] = false;
                        key_timers_[i] = 0.0f;
                    }
                }
            } else {
                key_timers_[i] = 0.0f;
            }
        }
    }


    void Application::on_key_down(unsigned char key) {
        int modifiers = glutGetModifiers();
        shift_pressed_ = (modifiers & GLUT_ACTIVE_SHIFT) != 0;
        ctrl_pressed_ = (modifiers & GLUT_ACTIVE_CTRL) != 0;
        alt_pressed_ = (modifiers & GLUT_ACTIVE_ALT) != 0;
        // Handle case-sensitive keys properly
        if (key >= 'A' && key <= 'Z') {
            unsigned char lower_key = key + ('a' - 'A'); // Convert to lowercase

            if (!keys_[key]) {
                keys_just_pressed_[key] = true;
            }
            if (!keys_[lower_key]) {
                keys_just_pressed_[lower_key] = true;
            }

            keys_[key] = true;
            keys_[lower_key] = true;

            key_timers_[key] = 0.0f;
            key_timers_[lower_key] = 0.0f;
        } else if (key >= 'a' && key <= 'z') {
            unsigned char upper_key = key - ('a' - 'A'); // Convert to uppercase

            if (!keys_[key]) {
                keys_just_pressed_[key] = true;
            }
            if (!keys_[upper_key]) {
                keys_just_pressed_[upper_key] = true;
            }

            keys_[key] = true;
            keys_[upper_key] = true;

            key_timers_[key] = 0.0f;
            key_timers_[upper_key] = 0.0f;
        } else {
            // Non-letter keys - handle normally
            if (!keys_[key]) {
                keys_just_pressed_[key] = true;
            }
            keys_[key] = true;
            key_timers_[key] = 0.0f;
        }
    }

    void Application::on_key_up(unsigned char key) {
        int modifiers = glutGetModifiers();
        shift_pressed_ = (modifiers & GLUT_ACTIVE_SHIFT) != 0;
        ctrl_pressed_ = (modifiers & GLUT_ACTIVE_CTRL) != 0;
        alt_pressed_ = (modifiers & GLUT_ACTIVE_ALT) != 0;

        // Handle case-sensitive keys properly
        if (key >= 'A' && key <= 'Z') {
            unsigned char lower_key = key + ('a' - 'A');

            if (keys_[key]) {
                keys_just_released_[key] = true;
            }
            if (keys_[lower_key]) {
                keys_just_released_[lower_key] = true;
            }

            keys_[key] = false;
            keys_[lower_key] = false;

            key_timers_[key] = 0.0f;
            key_timers_[lower_key] = 0.0f;
        } else if (key >= 'a' && key <= 'z') {
            unsigned char upper_key = key - ('a' - 'A');

            if (keys_[key]) {
                keys_just_released_[key] = true;
            }
            if (keys_[upper_key]) {
                keys_just_released_[upper_key] = true;
            }

            keys_[key] = false;
            keys_[upper_key] = false;

            key_timers_[key] = 0.0f;
            key_timers_[upper_key] = 0.0f;
        } else {
            if (keys_[key]) {
                keys_just_released_[key] = true;
            }
            keys_[key] = false;
            key_timers_[key] = 0.0f;
        }
    }

    void Application::on_special_key_down(int key) {
        int modifiers = glutGetModifiers();
        shift_pressed_ = (modifiers & GLUT_ACTIVE_SHIFT) != 0;
        ctrl_pressed_ = (modifiers & GLUT_ACTIVE_CTRL) != 0;
        alt_pressed_ = (modifiers & GLUT_ACTIVE_ALT) != 0;

        int key_index = key + 256;

        if (!keys_[key_index]) {
            keys_just_pressed_[key_index] = true;
        }
        keys_[key_index] = true;

        key_timers_[key_index] = 0.0f;
    }

    void Application::on_special_key_up(int key) {
        int modifiers = glutGetModifiers();
        shift_pressed_ = (modifiers & GLUT_ACTIVE_SHIFT) != 0;
        ctrl_pressed_ = (modifiers & GLUT_ACTIVE_CTRL) != 0;
        alt_pressed_ = (modifiers & GLUT_ACTIVE_ALT) != 0;

        int key_index = key + 256;

        if (keys_[key_index]) {
            keys_just_released_[key_index] = true;
        }
        keys_[key_index] = false;

        key_timers_[key_index] = 0.0f;
    }

    void Application::on_mouse_button(int button, int state, int x, int y) {
    }

    void Application::on_mouse_motion(int x, int y) {
        on_mouse_passive_motion(x, y);
    }

    void Application::on_mouse_passive_motion(int x, int y) {
        // if first mouse store the last x and y position.
        if (first_mouse_) {
            last_mouse_x_ = x;
            last_mouse_y_ = y;
            first_mouse_ = false;
            return;
        }

        // Calculate the offsets
        float x_offset = static_cast<float>(x - last_mouse_x_);
        float y_offset = static_cast<float>(last_mouse_y_ - y);

        // Ignore large jumps
        if (abs(x_offset) > 100 || abs(y_offset) > 100) {
            last_mouse_x_ = x;
            last_mouse_y_ = y;
            return;
        }

        last_mouse_x_ = x;
        last_mouse_y_ = y;

        if (callbacks_.on_mouse_moved) {
            callbacks_.on_mouse_moved(x_offset, y_offset);
        }

        // Warp when getting close to edges
        if (x < 100 || x > window_info_.width - 100 || y < 100 || y > window_info_.height - 100) {
            glutWarpPointer(window_info_.width / 2, window_info_.height / 2);
        }
    }

    void Application::on_mouse_wheel(int wheel, int direction, int x, int y) {
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse) {
            return;
        }
    }

    void Application::clear_frame_input_flags() {
        for (int i = 0; i < 512; i++) {
            keys_just_pressed_[i] = false;
            keys_just_released_[i] = false;
        }
    }

    void Application::process_input() {
        check_key_timeouts();

        if (callbacks_.process_input) {
            callbacks_.process_input(*this, delta_time_);
        }

        // Standard exit with ESC
        if (keys_[27]) {
            glutLeaveMainLoop();
        }

        // Only update previous key states
        for (int i = 0; i < 512; i++) {
            prev_keys_[i] = keys_[i];
        }
    }

    void Application::on_window_resize(int width, int height) {
        // Avoid unnecessary updates
        if (window_info_.width == width && window_info_.height == height) return;
        window_info_.width = width;
        window_info_.height = height;

        window_info_.aspect_ratio = static_cast<float>(width) / static_cast<float>(height);

        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(width);
        io.DisplaySize.y = static_cast<float>(height);

        glViewport(0, 0, width, height);

        for (auto &camera: scene_manager_.get_camera_entities()) {
            camera->get_component<CameraComponent>()->set_aspect_ratio(window_info_.aspect_ratio);
        }
    }

    void Application::load_scene() {
        if (callbacks_.setup) {
            callbacks_.setup(scene_manager_, window_info_, shader_manager_);
        } else {
            scene_manager_.create_default_scene();
        }
    }

    void Application::toggle_fullscreen() {
        static bool is_transitioning = false;
        if (is_transitioning) return;

        is_transitioning = true;

        static int prev_width = window_info_.width;
        static int prev_height = window_info_.height;
        static int prev_x = glutGet(GLUT_WINDOW_X);
        static int prev_y = glutGet(GLUT_WINDOW_Y);

        if (is_fullscreen_) {
            glutReshapeWindow(prev_width, prev_height);
            glutPositionWindow(prev_x, prev_y);
            is_fullscreen_ = false;
        } else {
            // Store the previous state into static variables
            prev_width = window_info_.width;
            prev_height = window_info_.height;
            prev_x = glutGet(GLUT_WINDOW_X);
            prev_y = glutGet(GLUT_WINDOW_Y);

            // Switch to fullscreen
            glutFullScreen();
            is_fullscreen_ = true;
        }
        window_info_.is_fullscreen = is_fullscreen_;

        // Allow transitions to happen after a short delay
        glutTimerFunc(200, [](int value) {
            if (value == 1) is_transitioning = false;
        }, 1);
    }

    Shader *Application::ensure_fallback_shader() {
        // Try to load a default shader
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

    Application *Application::instance_ = nullptr;
}
