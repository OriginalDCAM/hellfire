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
    glDisable(GL_CULL_FACE);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(app.get_window_width() / 2, app.get_window_height() / 2);

    uint32_t shader_program = app.create_shader_program("assets/shaders/vertexshader.vert",
                                                        "assets/shaders/fragmentshader.frag");

    app.toggle_fullscreen();
    app.set_shader_program(shader_program);
}

// Animation setup 
void Game::setup_animations() {
    // God creature animation
    DCraft::Object3D* god = scenes_["Terrain"]->find_object_by_name("Godly Creature");
    if (god) {
        if (auto* icosphere = god->find_object_by_name("Icosphere")) {
            animation_system_.create_rotation(
                "god_sphere_rotation",
                icosphere,    
                40.0f,    
                30.0f,     
                50.0f     
            );
        }
        
        if (auto* god_head = god->find_object_by_name("Icosphere")) {
            animation_system_.create_pulsing_scale(
                "icosphere_god__head_scaling",
                god_head,    
                1.0f,    
                1.0f,     
                1.75f     
            );
        }
        
        animation_system_.create_circular_path(
            "god_path", god, 30.0f, 15.0f, true);
    }

    // Mars rotation animation
    if (auto* mars = scenes_["Terrain"]->find_object_by_name("Mars 2K.obj")) {
        animation_system_.create_rotation("mars rotation around itself", mars, 0.0f, 10.0f, 0.0f);
    }

    DCraft::Light* point_light = static_cast<DCraft::Light*>(
    scenes_["Terrain"]->find_object_by_name("Green Light"));
    
    if (point_light) {
        animation_system_.create_intensity_pulse(
            "light_flicker",     
            point_light,         
            1.5f,                 
            0.7f,                 
            1.0f                  
        );
    }
}

void Game::on_scene_activated(DCraft::Scene *scene) {
    // Find the scene name from the scene pointer
    std::string scene_name;
    for (const auto &pair: scenes_) {
        if (pair.second == scene) {
            scene_name = pair.first;
            break;
        }
    }

    if (scene_name.empty()) {
        std::cerr << "Unknown scene activated!\n";
        return;
    }

    active_scene_name_ = scene_name;

    // Set the appropriate camera based on the current drone mode
    auto &cameras = scene_cameras_[scene_name];
    if (drone_mode_active) {
        if (cameras.drone_camera) {
            scene->set_active_camera(cameras.drone_camera);
        }
    } else {
        if (cameras.main_camera) {
            scene->set_active_camera(cameras.main_camera);
        }
    }

    // Update camera visuals
    const glm::vec3 visible_scale(0.3f);
    const glm::vec3 hidden_scale(0.0f);

    if (cameras.drone_camera_visual) {
        cameras.drone_camera_visual->set_scale(drone_mode_active ? hidden_scale : visible_scale);
    }
    if (cameras.main_camera_visual) {
        cameras.main_camera_visual->set_scale(drone_mode_active ? visible_scale : hidden_scale);
    }

    std::clog << "Activated scene: " << scene_name << "\n";
}

void Game::setup_cameras_for_scene(const std::string &scene_name) {
    if (scenes_.find(scene_name) == scenes_.end()) {
        std::cerr << "Scene " << scene_name << " not found!\n";
        return;
    }

    auto scene = scenes_[scene_name];
    SceneCameras cameras;

    // Find cameras and their visual representations in this scene
    cameras.main_camera = dynamic_cast<DCraft::PerspectiveCamera *>(scene->find_object_by_name("Main Camera"));
    cameras.drone_camera = dynamic_cast<DCraft::PerspectiveCamera *>(scene->find_object_by_name("Drone Camera"));
    cameras.main_camera_visual = scene->find_object_by_name("Main Camera Visual");
    cameras.drone_camera_visual = scene->find_object_by_name("Drone Camera Visual");

    // Validate that we found all required objects
    if (!cameras.main_camera || !cameras.drone_camera) {
        std::cerr << "Warning: Missing camera in scene " << scene_name << "!\n";
    }

    if (!cameras.main_camera_visual || !cameras.drone_camera_visual) {
        std::cerr << "Warning: Missing camera visual in scene " << scene_name << "!\n";
    }

    // Store the cameras for this scene
    scene_cameras_[scene_name] = cameras;
}


void Game::setup(DCraft::SceneManager &sm, DCraft::WindowInfo window) {
    scene_manager_ = &sm;

    // Scene loading with code behind
    scenes_["Sandbox"] = load_scene(sm, window);
    // Scene loading from file path
    scenes_["Terrain"] = sm.load_scene("assets/scenes/Test Scene.json");

    setup_cameras_for_scene("Sandbox");
    setup_cameras_for_scene("Terrain");

    // Set the initial active scene
    sm.set_active_scene(scenes_["Sandbox"]);
    active_scene_name_ = "Sandbox";

    // Set up the animations
    setup_animations();

    // Register scene activation callback with the scene manager
    sm.set_scene_activated_callback([this](DCraft::Scene *scene) {
        on_scene_activated(scene);
    });
}

Game::SceneCameras &Game::get_active_scene_cameras() {
    return scene_cameras_[active_scene_name_];
}

void Game::handle_input(DCraft::Application &app, float delta_time) {
    if (drone_toggle_timer > 0.0f) {
        drone_toggle_timer -= delta_time;
    }

    // Camera movement controls
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

    // Toggle drone mode
    if (app.is_key_pressed('v') || app.is_key_pressed('V')) {
        if (!drone_key_processed && drone_toggle_timer <= 0.0f) {
            drone_mode_active = !drone_mode_active;
            auto &cameras = get_active_scene_cameras();

            const glm::vec3 visible_scale(0.3f);
            const glm::vec3 hidden_scale(0.0f);

            if (drone_mode_active && cameras.drone_camera) {
                scene_manager_->get_active_scene()->set_active_camera(cameras.drone_camera);
                std::clog << "Entering drone mode with the " << scene_manager_->get_active_camera()->get_name() << '\n';
            } else if (cameras.main_camera) {
                scene_manager_->get_active_scene()->set_active_camera(cameras.main_camera);
                std::clog << "Leaving drone mode to the " << scene_manager_->get_active_camera()->get_name() << '\n';
            }

            if (cameras.drone_camera_visual) {
                cameras.drone_camera_visual->set_scale(drone_mode_active ? hidden_scale : visible_scale);
            }
            if (cameras.main_camera_visual) {
                cameras.main_camera_visual->set_scale(drone_mode_active ? visible_scale : hidden_scale);
            }

            drone_toggle_timer = DRONE_TOGGLE_COOLDOWN;
            drone_key_processed = true;
        }
    } else {
        drone_key_processed = false;
    }

    if (app.is_key_pressed('1')) {
    scene_manager_->set_active_scene(scenes_["Terrain"]);
        
    } else if (app.is_key_pressed('2')) {
    scene_manager_->set_active_scene(scenes_["Sandbox"]);
    }

    if (app.is_special_key_pressed(static_cast<char>(GLUT_KEY_F11 + 256))) {
        app.toggle_fullscreen();
    }
}

void Game::process_mouse_movement(float x_offset, float y_offset) {
    scene_manager_->get_active_camera()->process_mouse_movement(x_offset, y_offset);
}

void Game::process_camera_movement() {
    auto &cameras = get_active_scene_cameras();

    if (!cameras.main_camera || !cameras.drone_camera ||
        !cameras.main_camera_visual || !cameras.drone_camera_visual) {
        return;
    }

    // Update main camera visual
    cameras.main_camera_visual->set_position(cameras.main_camera->get_position());
    glm::vec3 camera_pos = cameras.main_camera->get_position();
    glm::vec3 camera_target = camera_pos + cameras.main_camera->get_front_vector();
    cameras.main_camera_visual->look_at(camera_target, cameras.main_camera->get_up_vector());
    cameras.main_camera_visual->match_orientation(*cameras.main_camera);

    // Update drone camera visual
    cameras.drone_camera_visual->set_position(cameras.drone_camera->get_position());
    glm::vec3 drone_pos = cameras.drone_camera->get_position();
    glm::vec3 drone_target = drone_pos + cameras.drone_camera->get_front_vector();
    cameras.drone_camera_visual->look_at(drone_target, cameras.drone_camera->get_up_vector());
    cameras.drone_camera_visual->match_orientation(*cameras.drone_camera);
}

void Game::update(float delta_time) {
    animation_system_.update(delta_time);
    
    process_camera_movement();

    for (auto &scene: scenes_) {
        scene.second->update(delta_time);
    }
}
