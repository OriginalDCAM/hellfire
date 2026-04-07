#include "DCraft/Application.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "imgui/backends/imgui_impl_glut.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/imgui.h"

#include "DCraft/Graphics/OGL/glsl.h"
#include "DCraft/Graphics/Renderer.h"
#include "DCraft/Addons/SceneManager.h"
#include "DCraft/Structs/Scene.h"

#include <iostream>
#include <utility>

#include "DCraft/Editor/SceneEditorOverlay.h"

namespace DCraft {
    Application::Application(int width, int height, std::string title) : renderer_(), title_(std::move(title)),
                                                                         editor_overlay_(&renderer_, scene_manager_),
                                                                         editor_mode_(false), game_mode_(true) {
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

        for (auto *camera: cameras_) {
            delete camera;
        }

        // Cleanup application
        if (instance_ == this) {
            instance_ = nullptr;
        }

        std::clog << "Application destroyed" << std::endl;
    }

    void Application::initialize(int argc, char **argv) {
        std::clog << "Initializing application..." << '\n';

        // Initialize GLUT
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
        glutInitWindowSize(window_info_.width, window_info_.height);
        glutCreateWindow(title_.c_str());
        std::clog << "GLUT initialization succeeded" << '\n';

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
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.IniFilename = "editorconfig.ini";
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 

        io.DisplaySize.x = static_cast<float>(window_info_.width);
        io.DisplaySize.y = static_cast<float>(window_info_.height);

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGLUT_Init();
        ImGui_ImplOpenGL3_Init("#version 330");

        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_CONTINUE_EXECUTION);

        if (callbacks_.init) {
            callbacks_.init(*this);
        }

        uint32_t fallback_shader = ensure_fallback_shader();
        renderer_.set_fallback_shader(fallback_shader);
        renderer_.init();
        std::clog << "Renderer setup succeeded" << '\n';

        // Create initial framebuffer for editor
        renderer_.create_scene_framebuffer(800, 600);

        load_scene();

        std::clog << "Application initialization complete" << std::endl;
    }

    void Application::setup_callbacks() {
        glutDisplayFunc(display_callback);
        glutTimerFunc(16, timer_callback, 0);
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

        glutPostRedisplay();
    }

    void Application::render_frame() {
        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGLUT_NewFrame();
        ImGui::NewFrame();

        Scene *active_scene = scene_manager_.get_active_scene();
        if (!active_scene) {
            std::cerr << "No active scene to render!" << std::endl;
            return;
        }

        Camera *active_camera = active_scene->get_active_camera();
        if (!active_camera) {
            std::cerr << "No active camera in scene!" << std::endl;
            return;
        }

        if (!game_mode_) {
            // Editor mode - render scene to framebuffer and show UI
            editor_overlay_.set_active_scene(active_scene);
            editor_overlay_.set_active_camera(active_camera);

            renderer_.set_render_to_framebuffer(true);

            editor_overlay_.render();
        } else {
            renderer_.set_render_to_framebuffer(false);

            renderer_.begin_frame();

            renderer_.render(*active_scene, *active_camera);

            renderer_.end_frame();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glutSwapBuffers();
    }

    void Application::on_key_down(unsigned char key) {
        if (key == '`') {
            keys_['`'] = true;
            return;
        }

        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureKeyboard) {
            return;
        }

        keys_[key] = true;
    }

    void Application::on_key_up(unsigned char key) {
        if (key == '`') {
            keys_['`'] = false;
            return;
        }

        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureKeyboard) {
            return;
        }
        keys_[key] = false;
    }

    void Application::on_special_key_down(int key) {
        if (key == '`') {
            keys_['`'] = true;
            return;
        }

        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureKeyboard) {
            return;
        }
        keys_[key + 256] = true;
    }

    void Application::on_special_key_up(int key) {
        if (key == '`') {
            keys_['`'] = false;
            return;
        }
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureKeyboard) {
            return;
        }
        keys_[key + 256] = false;
    }

    void Application::on_mouse_button(int button, int state, int x, int y) {
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse) {
            return;
        }
    }

    void Application::on_mouse_motion(int x, int y) {
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse) {
            return;
        }

        on_mouse_passive_motion(x, y);
    }

    void Application::on_mouse_passive_motion(int x, int y) {
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse || !game_mode_) {
            return;
        }
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

        for (auto &camera: scene_manager_.get_cameras()) {
            camera->set_aspect_ratio(window_info_.aspect_ratio);
        }
    }

    void Application::load_scene() {
        if (callbacks_.setup) {
            callbacks_.setup(scene_manager_, window_info_, shader_manager_);
        } else {
            scene_manager_.create_default_scene();
        }
    }

    void Application::process_input() {
        if (callbacks_.process_input) {
            callbacks_.process_input(*this, delta_time_);
        }

        // Handle switching to the Editor UI
        if (keys_['`'] && !prev_keys_['`']) {
            toggle_editor_mode();
        }

        if ((keys_[GLUT_KEY_F1 + 256] && !prev_keys_[GLUT_KEY_F1 + 256]) || // F1
            (keys_['t'] && !prev_keys_['t']) || // T key
            (keys_[9] && !prev_keys_[9])) {
            // TAB key
            toggle_editor_mode();
        }

        // Standard exit with ESC
        if (keys_[27] && game_mode_) {
            glutLeaveMainLoop();
        }

        for (int i = 0; i < 512; i++) {
            prev_keys_[i] = keys_[i];
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

    uint32_t Application::ensure_fallback_shader() {
        // Try to load a default shader, create basic one if needed
        try {
            return shader_manager_.load_shader_from_files(
                "assets/shaders/standard.vert",
                "assets/shaders/lambert.frag"
            );
        } catch (const std::exception &e) {
            std::cerr << "Warning: Could not load default shaders: " << e.what() << std::endl;
            std::cerr << "Using minimal fallback shader" << std::endl;

            // Create a very basic shader programmatically or return 0
            return 0;
        }
    }

    void Application::toggle_editor_mode() {
        game_mode_ = !game_mode_;

        if (game_mode_) {
            glutSetCursor(GLUT_CURSOR_NONE);
            glutWarpPointer(window_info_.width / 2, window_info_.height / 2);
            first_mouse_ = true; // Reset mouse tracking

            glViewport(0, 0, window_info_.width, window_info_.height);
            std::clog << "Switched to Game Mode" << std::endl;
        } else {
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

            std::clog << "Switched to Editor Mode" << std::endl;
        }
    }

    Application *Application::instance_ = nullptr;
}
