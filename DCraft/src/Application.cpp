#include "DCraft/Application.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Dcraft/Graphics/OGL/glsl.h"
#include <iostream>

namespace DCraft {
    Application::Application(int width, int height, const std::string &title) :
        title_(title) {
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
        delete renderer_;

        for (auto *camera: cameras_) {
            delete camera;
        }

        for (uint32_t sp: shader_programs_) {
            glDeleteProgram(sp);
        }

        // Cleanup application
        if (instance_ == this) {
            instance_ = nullptr;
        }

        std::clog << "Application destroyed" << std::endl;
    }

    uint32_t Application::create_shader_program(const std::string &vertex_path, const std::string &fragment_path) {
        std::string base_path = "";

        #ifdef _WIN32
        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        base_path = std::string(exePath);
        base_path = base_path.substr(0, base_path.find_last_of("\\/")) + "/";
        #endif

        std::string abs_vertex_path = base_path + vertex_path;
        std::string abs_fragment_path = base_path + fragment_path;

        std::cout << "Loading vertex shader from: " << abs_vertex_path << std::endl;
        std::cout << "Loading fragment shader from: " << abs_fragment_path << std::endl;

        // Read shader files
        char *vertex_source = glsl::readFile(abs_vertex_path.c_str());
        if (!vertex_source) {
            std::cerr << "Failed to read vertex shader" << std::endl;
            return 0;
        }

        GLuint vertex_shader = glsl::makeVertexShader(vertex_source);
        delete[] vertex_source; // Free the memory

        if (vertex_shader == 0) {
            return 0;
        }

        char *fragment_source = glsl::readFile(abs_fragment_path.c_str());
        if (!fragment_source) {
            std::cerr << "Failed to read fragment shader" << std::endl;
            glDeleteShader(vertex_shader);
            return 0;
        }

        GLuint fragment_shader = glsl::makeFragmentShader(fragment_source);
        delete[] fragment_source; // Free the memory

        if (fragment_shader == 0) {
            glDeleteShader(vertex_shader);
            return 0;
        }

        uint32_t program_id = glsl::makeShaderProgram(vertex_shader, fragment_shader);

        // Shaders are no longer needed after the program is linked
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        if (program_id != 0) {
            // Store for cleanup later
            shader_programs_.push_back(program_id);
        }

        return program_id;
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

        glutSetCursor(GLUT_CURSOR_NONE);
        glutWarpPointer(window_info_.width / 2, window_info_.height / 2);

        // Setup OpenGL state
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        // Setup GLUT callbacks
        setup_callbacks();
        std::clog << "GLUT callbacks setup succeeded" << '\n';

        if (callbacks_.init) {
            callbacks_.init(*this);
        }

        renderer_ = new Renderer(shader_program_id_);
        std::clog << "Renderer setup succeeded" << '\n';

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

    void Application::render_frame() const {
        // Clear the screen
        renderer_->begin_frame();

        renderer_->render(*scene_manager_.get_root_node(), *scene_manager_.get_active_camera());

        renderer_->end_frame();

        glutSwapBuffers();
    }

    void Application::on_key_down(unsigned char key) {
        keys_[key] = true;
    }

    void Application::on_key_up(unsigned char key) {
        keys_[key] = false;
    }

    void Application::on_special_key_down(int key) {
        keys_[key + 256] = true;
    }

    void Application::on_special_key_up(int key) {
        keys_[key + 256] = false;
    }

    void Application::on_mouse_button(int button, int state, int x, int y) {
    }

    void Application::on_mouse_motion(int x, int y) {
        // For now
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
    }

    void Application::on_window_resize(int width, int height) {
        // Avoid unnecessary updates
        if (window_info_.width == width && window_info_.height == height) return;
        window_info_.width = width;
        window_info_.height = height;

        window_info_.aspect_ratio = static_cast<float>(width) / static_cast<float>(height);

        glViewport(0, 0, width, height);

        for (auto& camera : scene_manager_.get_cameras()) {
            camera->set_aspect_ratio(window_info_.aspect_ratio);
        }
    }

    void Application::load_scene() {
        if (callbacks_.setup) {
            callbacks_.setup(scene_manager_, window_info_);
        } else {
            scene_manager_.create_default_scene();
        }
    }

    void Application::process_input() {

        if (callbacks_.process_input) {
            callbacks_.process_input(*this, delta_time_);
        }

        if (keys_[27]) { // ESC key

            glutLeaveMainLoop(); 
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

    Application *Application::instance_ = nullptr;
}
