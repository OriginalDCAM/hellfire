#include <DCraft/Application.h>
#include <DCraft/Graphics/GL/glsl.h>
#include <Dcraft/Graphics/Primitives/Cube.h>
#include <Dcraft/Structs/Scene.h>
#include <DCraft/Graphics/Primitives/Plane.h>

class Game {
public:
    Game() {
    }

    ~Game() {
        delete cube_;
    }

    bool drone_key_processed = false;
    bool drone_mode_active = false;
    float drone_toggle_timer = 0.0f;
    const float DRONE_TOGGLE_COOLDOWN = 0.2f; // 200ms cooldown

    void init(DCraft::Application &app) {
        glutSetCursor(GLUT_CURSOR_NONE);
        glutWarpPointer(app.get_window_width() / 2, app.get_window_height() / 2);

        glDisable(GL_CULL_FACE);

        uint32_t shader_program = app.create_shader_program("assets/shaders/vertexshader.vert",
                                                            "assets/shaders/fragmentshader.frag");

        app.set_shader_program(shader_program);
    }

    void setup(DCraft::SceneManager &sm, DCraft::WindowInfo window) {
        scene_manager_ = &sm;

        initial_scene = sm.create_scene("GameScene");

        initial_scene->set_position(0.0f, 0.0f, 0.0f);

        camera_ = initial_scene->create_camera<DCraft::PerspectiveCamera>(
            "MainCamera", 70.0f, window.aspect_ratio, 0.1f, 100.0f);
        camera_->set_position(0.0f, 2.0f, 10.0f);
        initial_scene->add(camera_);
        initial_scene->set_active_camera(camera_);

        drone_camera_ = initial_scene->create_camera<DCraft::PerspectiveCamera>(
            "DroneCamera", 70.0, window.aspect_ratio, 0.1f, 200.0f);
        drone_camera_->set_position(10.0f, 25.0f, 10.0f);
        drone_camera_->set_target(.0f, 1.0f, 0.0f);
        initial_scene->add(drone_camera_);

        cube_ = new DCraft::Cube();
        cube_->set_position(0.0f, 1.0f, 0.0f);
        initial_scene->add(cube_);

        DCraft::Plane *plane = new DCraft::Plane();
        plane->set_position(0, -10, 0);
        plane->set_scale(glm::vec3(100.0f, 100.0f, 100.0f));
        plane->set_rotation(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        initial_scene->add(plane);
        sm.set_active_scene(initial_scene);
    }

    void handle_input(DCraft::Application &app, float delta_time) {
        if (drone_toggle_timer > 0.0f) {
            drone_toggle_timer -= delta_time;
        }

        if (app.is_key_pressed('w') || app.is_key_pressed('W')) {
            scene_manager_->get_active_camera()->process_keyboard(DCraft::FORWARD, delta_time);
        }
        if (app.is_key_pressed('s') || app.is_key_pressed('S')) {
            scene_manager_->get_active_camera()->process_keyboard(DCraft::BACKWARD, delta_time);
        }
        if (app.is_key_pressed('a') || app.is_key_pressed('A')) {
            scene_manager_->get_active_camera()->process_keyboard(DCraft::LEFT, delta_time);
        }
        if (app.is_key_pressed('d') || app.is_key_pressed('D')) {
            scene_manager_->get_active_camera()->process_keyboard(DCraft::RIGHT, delta_time);
        }
        if (app.is_key_pressed('q') || app.is_key_pressed('Q')) {
            scene_manager_->get_active_camera()->process_keyboard(DCraft::DOWN, delta_time);
        }
        if (app.is_key_pressed('e') || app.is_key_pressed('E')) {
            scene_manager_->get_active_camera()->process_keyboard(DCraft::UP, delta_time);
        }
        if (app.is_key_pressed('z') || app.is_key_pressed('Z')) {
            if (!drone_key_processed && drone_toggle_timer <= 0.0f) {
                drone_mode_active = !drone_mode_active;

                if (drone_mode_active) {
                    initial_scene->set_active_camera(drone_camera_);
                    std::clog << "Entering drone mode with the " << '\n';
                } else {
                    initial_scene->set_active_camera(camera_);
                    std::clog << "leaving drone mode with the " << '\n';
                }
                drone_toggle_timer = DRONE_TOGGLE_COOLDOWN;
                drone_key_processed = true;
            }
        } else {
            drone_key_processed = false;
        }

        if (app.is_special_key_pressed(GLUT_KEY_F11 + 256)) {
            app.toggle_fullscreen();
        }

        if (app.is_key_pressed('1')) {
            cube_->set_position(0.0f, 0.0f, 0.0f);
        }
    }

    void update(float delta_time) {
        cube_->update(delta_time);
    }

private:
    DCraft::PerspectiveCamera *camera_ = nullptr;
    DCraft::PerspectiveCamera *drone_camera_ = nullptr;
    std::vector<DCraft::Object3D *> objects_;
    DCraft::Cube *cube_ = nullptr;
    DCraft::Scene *initial_scene;
    DCraft::SceneManager *scene_manager_;
};

int main(int argc, char **argv) {
    DCraft::Application app(800, 600, "Final assignment :)");

    DCraft::ApplicationCallbacks application_callbacks;

    Game game;
    application_callbacks.setup = [&game](DCraft::SceneManager &sm, const DCraft::WindowInfo &info) {
        game.setup(sm, info);
    };
    application_callbacks.init = [&game](DCraft::Application &app) { game.init(app); };
    application_callbacks.update = [&game](float dt) { game.update(dt); };
    application_callbacks.process_input = [&game](DCraft::Application &app, float dt) { game.handle_input(app, dt); };

    app.set_callbacks(application_callbacks);

    // Initialize and run
    app.initialize(argc, argv);
    app.run();

    return 0;
}
