//
// Created by denzel on 03/04/2025.
//

#include <Dcraft/Graphics/Geometry/Cube.h>

#include "Game.h"
#include "Scenes/SolarSystemScene.h"
#include "Scripts/PlayerController.h"


void Game::setup_callbacks(DCraft::Application &app) {
    DCraft::ApplicationCallbacks application_callbacks;

    application_callbacks.init = [this](DCraft::Application &app) { init(app); };
    application_callbacks.setup = [this](DCraft::SceneManager &sm, const DCraft::WindowInfo &info, DCraft::ShaderManager &shader_manager) {
        setup(sm, info, shader_manager);
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

    app.toggle_fullscreen();
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

    std::clog << "Activated scene: " << scene_name << "\n";
}

void Game::setup_cameras_for_scene(const std::string &scene_name) {
    if (scenes_.find(scene_name) == scenes_.end()) {
        std::cerr << "Scene " << scene_name << " not found!\n";
        return;
    }

    SceneCameras cameras;

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


void Game::setup(DCraft::SceneManager &sm, DCraft::WindowInfo window, DCraft::ShaderManager &shader_manager) {
    scene_manager_ = &sm;

    // Scene loading with code behind
    scenes_["SolarSystem"] = load_solar_system_scene(sm, window, shader_manager);

    // Set the initial active scene
    sm.set_active_scene(scenes_["SolarSystem"]);
    active_scene_name_ = "SolarSystem";

    // Register scene activation callback with the scene manager
    sm.set_scene_activated_callback([this](DCraft::Scene *scene) {
        on_scene_activated(scene);
    });
}

Game::SceneCameras &Game::get_active_scene_cameras() {
    return scene_cameras_[active_scene_name_];
}

void Game::handle_input(DCraft::Application &app, float delta_time) {
    if (app.is_special_key_pressed(static_cast<char>(GLUT_KEY_F11 + 256))) {
        app.toggle_fullscreen();
    }
}

void Game::process_mouse_movement(float x_offset, float y_offset) {
    // Find the camera entity with PlayerController and forward mouse movement
    if (!scene_manager_ || scenes_.empty()) {
        return;
    }

    auto* active_scene = scene_manager_->get_active_scene();
    if (!active_scene) {
        return;
    }

    // Look for the "Main Camera" entity 
    if (auto* camera_entity = active_scene->find_entity_by_name("Main Camera")) {
        if (auto* player_controller = camera_entity->get_component<PlayerController>()) {
            player_controller->handle_mouse_movement(x_offset, y_offset);
        }
    }

}

void Game::update(float delta_time) {
    animation_system_.update(delta_time);

    for (auto &scene: scenes_) {
        scene.second->update(delta_time);
    }
}