//
// Created by denzel on 03/04/2025.
//

#include <Dcraft/Graphics/Primitives/Cube.h>
#include <DCraft/Graphics/Primitives/Plane.h>

#include "Game.h"


void Game::setup_callbacks(DCraft::Application &app) {
    DCraft::ApplicationCallbacks application_callbacks;

    application_callbacks.setup = [this](DCraft::SceneManager &sm, const DCraft::WindowInfo &info) {
        setup(sm, info);
    };
    application_callbacks.init = [this](DCraft::Application &app) { init(app); };
    application_callbacks.update = [this](float dt) { update(dt); };
    application_callbacks.process_input = [this](DCraft::Application &app, float dt) {
        handle_input(app, dt);
    };

    app.set_callbacks(application_callbacks);
}

void Game::init(DCraft::Application &app) {
    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(app.get_window_width() / 2, app.get_window_height() / 2);

    glDisable(GL_CULL_FACE);

    uint32_t shader_program = app.create_shader_program("assets/shaders/vertexshader.vert",
                                                        "assets/shaders/fragmentshader.frag");
    
    app.toggle_fullscreen();
    app.set_shader_program(shader_program);
}

void Game::setup(DCraft::SceneManager &sm, DCraft::WindowInfo window) {
    scene_manager_ = &sm;

    // SceneLoader scene_loader("pathtofile.json");

    initial_scene = sm.create_scene("Test Scene");

    initial_scene->set_position(0.0f, 0.0f, 0.0f);

    // Main camera
    camera_ = initial_scene->create_camera<DCraft::PerspectiveCamera>(
        "Main Camera", 70.0f, window.aspect_ratio, 0.1f, 400.0f);
    camera_->set_position(0.0f, 2.0f, 10.0f);
    initial_scene->add(camera_);
    initial_scene->set_active_camera(camera_);

    // Drone camera
    drone_camera_ = initial_scene->create_camera<DCraft::PerspectiveCamera>(
        "Drone Camera", 70.0, window.aspect_ratio, 0.1f, 200.0f);
    drone_camera_->set_position(10.0f, 25.0f, 10.0f);
    drone_camera_->set_target(.0f, 1.0f, 0.0f);
    initial_scene->add(drone_camera_);

    // Cube primitive object
    cube_ = new DCraft::Cube();
    cube_->set_name("Cool cube");
    cube_->set_rotation(90, glm::vec3(1.0f, 0.0f, 0.0f));
    cube_->set_scale(glm::vec3(5.0f, 5.0f, 5.0f));
    cube_->set_position(0.0f, 1.0f, 0.0f);
    cube_->set_texture("assets/textures/mossy_brick.jpg", DCraft::TextureType::DIFFUSE);
    initial_scene->add(cube_);

    // Plane primitive object
    DCraft::Plane *plane = new DCraft::Plane();
    plane->set_name("Ground");
    plane->set_position(0, -10, 0);
    plane->set_scale(glm::vec3(100.0f, 100.0f, 100.0f));
    plane->set_rotation(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    plane->set_texture("assets/textures/brick_pavement.jpg", DCraft::TextureType::DIFFUSE);
    initial_scene->add(plane);

    sm.set_active_scene(initial_scene);
}

void Game::handle_input(DCraft::Application &app, float delta_time) {
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
                std::clog << "Entering drone mode with the " << initial_scene->get_active_camera()->get_name() <<
                        '\n';
            } else {
                initial_scene->set_active_camera(camera_);
                std::clog << "Leaving drone mode to the " << initial_scene->get_active_camera()->get_name() << '\n';
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

void Game::update(float delta_time) const {
    initial_scene->update(delta_time);
}
