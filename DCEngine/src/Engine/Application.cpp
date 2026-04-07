#include "Application.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Core/Graphics/GL/glsl.h"
#include <iostream>

namespace DCraft
{

    Application::Application(int width, int height, const std::string& title) : width_(width), height_(height), title_(title)
    {
        if (instance_ != nullptr)
        {
            throw std::runtime_error("Singleton Application already created");
        }
        instance_ = this;
    }

    Application::~Application()
    {
        delete renderer_;

        for (auto* camera : cameras_)
        {
            delete camera;
            camera = nullptr;
        }

        for (uint32_t sp : shader_programs_)
        {
            glDeleteProgram(sp);
        }

        // Cleanup application
        if (instance_ == this)
        {
            instance_ = nullptr;
        }

        std::clog << "Application destroyed" << std::endl;
    }

    uint32_t Application::create_shader_program(const std::string& vertex_path, const std::string& fragment_path)
    {
        char* vertexshader = glsl::readFile(vertex_path.c_str());
        GLuint vsh_id = glsl::makeVertexShader(vertexshader);
        char* fragshader = glsl::readFile(fragment_path.c_str());
        GLuint fsh_id = glsl::makeFragmentShader(fragshader);
        uint32_t program_id = glsl::makeShaderProgram(vsh_id, fsh_id);

        // Store for cleanup later
        shader_programs_.push_back(program_id);

        return program_id;
    }


    void Application::configure_camera(float fov, float near_plane, float far_plane, glm::vec3 position)
    {

    }

    void Application::initialize(int argc, char** argv)
    {
        std::clog << "Initializing application..." << '\n';
        // Initialize GLUT
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
        glutInitWindowSize(width_, height_);
        glutCreateWindow(title_.c_str());
        std::clog << "GLUT initialization succeeded" << '\n';

        // Initialize GLEW
        GLenum err = glewInit();
        if (err != GLEW_OK)
        {
            std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << '\n';
            exit(1);
        }
        std::clog << "GLEW initialization succeeded" << '\n';

        glutSetCursor(GLUT_CURSOR_NONE);
        glutWarpPointer(width_ / 2, height_ / 2);

        // Setup OpenGL state
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);


        // Setup GLUT callbacks
        setup_callbacks();

        if (callbacks_.init)
        {
            callbacks_.init(*this);
        }

        renderer_ = new Renderer(shader_program_id_);

        load_scene();

        std::clog << "Application initialization complete" << std::endl;

    }

    void Application::setup_callbacks()
    {
        glutDisplayFunc(display_callback);
        glutTimerFunc(16, timer_callback, 0);
        glutKeyboardFunc(key_down_callback);
        glutKeyboardUpFunc(key_up_callback);
        glutSpecialFunc(special_key_down_callback);
        glutSpecialUpFunc(special_key_up_callback);
        glutMouseFunc(mouse_button_callback);
        glutMotionFunc(mouse_motion_callback);
        glutPassiveMotionFunc(mouse_passive_motion_callback);
        glutMouseWheelFunc(mouse_wheel_callback);
    }

    void Application::run()
    {
        glutMainLoop();
    }

    void Application::update()
    {
        // Calculate delta time
        float current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
        delta_time_ = current_time - last_frame_time_;
        last_frame_time_ = current_time;

        process_input();

        scene_manager_.update(delta_time_);

        if (callbacks_.update)
        {
            callbacks_.update(delta_time_);
        }

        glutPostRedisplay();
    }

    void Application::render_frame()
    {
        // Clear the screen
        renderer_->begin_frame();

        renderer_->render(*scene_manager_.get_root_node(), *scene_manager_.get_active_camera());

        renderer_->end_frame();

        glutSwapBuffers();
    }

    void Application::on_key_down(unsigned char key, int x, int y)
    {
        keys_[key] = true;
    }

    void Application::on_key_up(unsigned char key, int x, int y)
    {
        keys_[key] = false;
    }

    void Application::on_special_key_down(int key, int x, int y)
    {
        //keys_[key + 256] = true;
    }

    void Application::on_special_key_up(int key, int x, int y)
    {
        //keys_[key + 256] = false;
    }

    void Application::on_mouse_button(int button, int state, int x, int y)
    {
    }

    void Application::on_mouse_motion(int x, int y)
    {
    }

    void Application::on_mouse_passive_motion(int x, int y)
    {
        if (first_mouse_)
        {
            last_mouse_x_ = x;
            last_mouse_y_ = y;
            first_mouse_ = false;
            return;
        }

        float x_offset = x - last_mouse_x_;
        float y_offset = last_mouse_y_ - y;

        last_mouse_x_ = x;
        last_mouse_y_ = y;

        scene_manager_.get_active_camera()->process_mouse_movement(x_offset, y_offset);

        if (x < width_ / 4 || x > width_ * 3 / 4 || y < height_ / 4 || y > height_ * 3 / 4) {
            glutWarpPointer(width_ / 2, height_ / 2);
            last_mouse_x_ = width_ / 2;
            last_mouse_y_ = height_ / 2;
        }

    }

    void Application::on_mouse_wheel(int wheel, int direction, int x, int y)
    {
    }

    void Application::load_scene()
    {
        if (callbacks_.setup)
        {
            WindowInfo info;
            info.width = width_;
            info.height = height_;
            info.aspect_ratio = static_cast<float>(width_) / height_;   
            callbacks_.setup(scene_manager_, info);
        }
        else
        {
            scene_manager_.create_default_scene();
        }

    }

    void Application::process_input()
    {
        if (keys_['w']) scene_manager_.get_active_camera()->process_keyboard(FORWARD, delta_time_);
        if (keys_['s']) scene_manager_.get_active_camera()->process_keyboard(BACKWARD, delta_time_);
        if (keys_['a']) scene_manager_.get_active_camera()->process_keyboard(LEFT, delta_time_);
        if (keys_['d']) scene_manager_.get_active_camera()->process_keyboard(RIGHT, delta_time_);
        if (keys_['q']) scene_manager_.get_active_camera()->process_keyboard(DOWN, delta_time_);
        if (keys_['e']) scene_manager_.get_active_camera()->process_keyboard(UP, delta_time_);
        if (keys_[27]) exit(1);
    }

    Application* Application::instance_ = nullptr;
}
