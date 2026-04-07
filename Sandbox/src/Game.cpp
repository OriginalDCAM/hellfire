//
// Created by denzel on 03/04/2025.
//

#include <Dcraft/Graphics/Primitives/Cube.h>
#include <DCraft/Graphics/Primitives/Plane.h>

#include "Game.h"

#include "Scenes/SandboxScene.h"


void Game::setup_callbacks(DCraft::Application &app) {
    DCraft::ApplicationCallbacks application_callbacks;

    application_callbacks.init = [this](DCraft::Application &app) { init(app); };
    application_callbacks.setup = [this](DCraft::SceneManager &sm, const DCraft::WindowInfo &info) {
        setup(sm, info);
    };
    application_callbacks.update = [this](float dt) { update(dt); };
    application_callbacks.process_input = [this](DCraft::Application &app, float dt) {
        handle_input(app, dt);
    };
    application_callbacks.on_mouse_moved = [this](float x_offset, float y_offset) {
        process_mouse_movement(x_offset, y_offset);
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
    
    scenes_["Sandbox"] = load_scene(sm, window);
    
    sm.set_active_scene(scenes_["Sandbox"]);
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
    if (app.is_key_pressed('v') || app.is_key_pressed('V')) {
        if (!drone_key_processed && drone_toggle_timer <= 0.0f) {
            drone_mode_active = !drone_mode_active;

            const glm::vec3 visible_scale(0.3f);
            const glm::vec3 hidden_scale(0.0f);
            if (drone_mode_active) {
                scene_manager_->get_active_scene()->set_active_camera(drone_camera_);
                std::clog << "Entering drone mode with the " << initial_scene->get_active_camera()->get_name() <<
                        '\n';
            } else {
                scene_manager_->get_active_scene()->set_active_camera(camera_);
                std::clog << "Leaving drone mode to the " << initial_scene->get_active_camera()->get_name() << '\n';
            }

            drone_camera_visual_->set_scale(drone_mode_active ? hidden_scale : visible_scale);
            main_camera_visual_->set_scale(drone_mode_active ? visible_scale : hidden_scale);

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

void Game::process_mouse_movement(float x_offset, float y_offset) {
    scene_manager_->get_active_camera()->process_mouse_movement(x_offset, y_offset);
}

void Game::process_camera_movement() {
    // Set the position to that of the camera

    // Get the cameras vector's
    glm::vec3 main_front = camera_->get_front_vector();
    glm::vec3 main_right = camera_->get_right_vector();
    glm::vec3 main_up = camera_->get_up_vector();

    // extract the rotation from the vector's
    glm::mat4 main_rotation(1.0f);
    main_rotation[0] = glm::vec4(main_right, 0.0f);
    main_rotation[1] = glm::vec4(main_up, 0.0f);
    main_rotation[2] = glm::vec4(main_front, 0.0f);

    // set the visuals rotation matrix to that of the camera to mimic the direction
    main_camera_visual_->set_rotation_matrix(main_rotation);

    glm::vec3 front = drone_camera_->get_front_vector();
    glm::vec3 right = drone_camera_->get_right_vector();
    glm::vec3 up = drone_camera_->get_up_vector();

    glm::mat4 drone_rotation(1.0f);
    drone_rotation[0] = glm::vec4(right, 0.0f);
    drone_rotation[1] = glm::vec4(up, 0.0f);
    drone_rotation[2] = glm::vec4(front, 0.0f);

    drone_camera_visual_->set_rotation_matrix(drone_rotation);
}

void Game::update(float delta_time) {
    for (auto& scene : scenes_) {
        scene.second->update(delta_time);
    }
}
