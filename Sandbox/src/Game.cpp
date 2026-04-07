//
// Created by denzel on 03/04/2025.
//

#include <Dcraft/Graphics/Primitives/Cube.h>
#include <DCraft/Graphics/Primitives/Plane.h>

#include "Game.h"


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

    DCraft::Material *mossy_material = new DCraft::Material();
    mossy_material->set_texture("assets/textures/mossy_brick.jpg", DCraft::TextureType::DIFFUSE);
    mossy_material->set_shader(shader_program);

    DCraft::Material *pavement_material = new DCraft::Material();
    pavement_material->set_texture("assets/textures/brick_pavement.jpg", DCraft::TextureType::DIFFUSE);
    pavement_material->set_shader(shader_program);


    DCraft::Material *miquel_material = new DCraft::Material();
    miquel_material->set_texture("assets/textures/miquel.jpg", DCraft::TextureType::DIFFUSE);
    miquel_material->set_shader(shader_program);


    DCraft::Material *plastic_green_material = new DCraft::Material();
    plastic_green_material->set_texture("assets/textures/plastic_green.jpg", DCraft::TextureType::DIFFUSE);
    plastic_green_material->set_shader(shader_program);


    DCraft::Material *denzel_material = new DCraft::Material();
    denzel_material->set_texture("assets/textures/denzel.jpg", DCraft::TextureType::DIFFUSE);
    denzel_material->set_shader(shader_program);


    DCraft::Material *plastic_blue_material = new DCraft::Material();
    plastic_blue_material->set_texture("assets/textures/plastic_blue.jpg", DCraft::TextureType::DIFFUSE);
    plastic_blue_material->set_shader(shader_program);

    materials_ = {
        {"PLASTIC_BLUE_MATERIAL", plastic_blue_material}, {"DENZEL_MATERIAL", denzel_material},
        {"PLASTIC_GREEN_MATERIAL", plastic_green_material}, {"MIQUEL_MATERIAL", miquel_material},
        {"MOSSY_MATERIAL", mossy_material}, {"PAVEMENT_MATERIAL", pavement_material}
    };
    
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

    // Create the drone camera
    drone_camera_ = initial_scene->create_camera<DCraft::PerspectiveCamera>(
        "Drone Camera", 70.0f, window.aspect_ratio, 0.1f, 400.0f);
    drone_camera_->set_position(10.0f, 25.0f, 10.0f);
    drone_camera_->set_target(0.0f, 0.0f, 0.0f);
    initial_scene->add(drone_camera_);

    main_camera_visual_ = new DCraft::Cube();
    main_camera_visual_->set_name("Main Camera Visual");
    main_camera_visual_->set_scale(glm::vec3(0.3f, 0.3f, 0.3f));
    main_camera_visual_->set_position(camera_->get_position());
    main_camera_visual_->set_material(materials_["MIQUEL_MATERIAL"]);
    initial_scene->add(main_camera_visual_);

    // Visual indicator for drone camera position
    drone_camera_visual_ = new DCraft::Cube();
    drone_camera_visual_->set_name("Drone Camera Visual");
    drone_camera_visual_->set_scale(glm::vec3(0.3f, 0.3f, 0.3f));
    drone_camera_visual_->set_position(drone_camera_->get_position());
    drone_camera_visual_->set_material(materials_["DENZEL_MATERIAL"]);

    DCraft::Cube *main_camera_direction = new DCraft::Cube();
    main_camera_direction->set_name("Main Camera Direction");
    main_camera_direction->set_scale(glm::vec3(0.05f, 0.05f, 0.5f));
    main_camera_direction->set_position(0.0f, 0.0f, 1.0f);
    main_camera_direction->set_material(materials_["PLASTIC_GREEN_MATERIAL"]);
    main_camera_visual_->add(main_camera_direction);

    // Direction indicator for drone camera
    DCraft::Cube *drone_camera_direction = new DCraft::Cube();
    drone_camera_direction->set_name("Drone Camera Direction");
    drone_camera_direction->set_scale(glm::vec3(0.05f, 0.05f, 0.5f));
    drone_camera_direction->set_position(0.0f, 0.0f, 1.0f);
    drone_camera_visual_->add(drone_camera_direction);
    drone_camera_direction->set_material(materials_["PLASTIC_GREEN_MATERIAL"]);
    initial_scene->add(drone_camera_visual_);

    cube_ = new DCraft::Cube();
    cube_->set_name("Cool cube");
    cube_->set_rotation(90, glm::vec3(1.0f, 0.0f, 0.0f));
    cube_->set_scale(glm::vec3(5.0f, 5.0f, 5.0f));
    cube_->set_position(0.0f, 1.0f, 0.0f);
    cube_->set_material(materials_["MOSSY_MATERIAL"]);
    initial_scene->add(cube_);

    // Plane primitive object
    DCraft::Plane *plane = new DCraft::Plane();
    plane->set_name("Ground");
    plane->set_position(0, -10, 0);
    plane->set_scale(glm::vec3(100.0f, 100.0f, 100.0f));
    plane->set_rotation(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    plane->set_material(materials_["PAVEMENT_MATERIAL"]);
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
    if (app.is_key_pressed('v') || app.is_key_pressed('V')) {
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

void Game::process_mouse_movement(float x_offset, float y_offset) {
    scene_manager_->get_active_camera()->process_mouse_movement(x_offset, y_offset);

}
void Game::process_camera_movement() {
    main_camera_visual_->set_position(camera_->get_position());
    drone_camera_visual_->set_position(drone_camera_->get_position());

    glm::vec3 main_front = camera_->get_front_vector();
    glm::vec3 main_right = camera_->get_right_vector();
    glm::vec3 main_up = camera_->get_up_vector();

    glm::mat4 main_rotation(1.0f);
    main_rotation[0] = glm::vec4(main_right, 0.0f);
    main_rotation[1] = glm::vec4(main_up, 0.0f);
    main_rotation[2] = glm::vec4(main_front, 0.0f);

    main_camera_visual_->set_rotation_matrix(main_rotation);

    glm::vec3 front = drone_camera_->get_front_vector();
    glm::vec3 right = drone_camera_->get_right_vector();
    glm::vec3 up = drone_camera_->get_up_vector();

    // Build rotation matrix from these orthogonal vectors
    glm::mat4 drone_rotation(1.0f);
    drone_rotation[0] = glm::vec4(right, 0.0f);
    drone_rotation[1] = glm::vec4(up, 0.0f);
    drone_rotation[2] = glm::vec4(front, 0.0f);

    drone_camera_visual_->set_rotation_matrix(drone_rotation);

    // Apply rotations to camera visualizers
    main_camera_visual_->set_rotation_matrix(main_rotation);

    // Hide active camera's visualizer
    if (drone_mode_active) {
        drone_camera_visual_->set_scale(glm::vec3(0.0f)); // Hide drone camera when using it
        main_camera_visual_->set_scale(glm::vec3(0.3f)); // Show main camera
    } else {
        drone_camera_visual_->set_scale(glm::vec3(0.3f)); // Show drone camera
        main_camera_visual_->set_scale(glm::vec3(0.0f)); // Hide main camera when using it
    }
}

void Game::update(float delta_time) {
    process_camera_movement();
    initial_scene->update(delta_time);
}
