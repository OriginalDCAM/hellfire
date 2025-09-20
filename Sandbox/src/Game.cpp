//
// Created by denzel on 03/04/2025.
//

#include "Game.h"

#include "DCraft/Utility/ServiceLocator.h"
#include "GLFW/glfw3.h"
#include "Scenes/SolarSystemScene.h"
#include "Scenes/SponzaScene.h"
#include "Scripts/PlayerController.h"
#include "Utils/AnimationInputHandler.h"


void Game::setup_callbacks(hellfire::Application &app) {
    hellfire::ApplicationCallbacks application_callbacks;

    application_callbacks.init = [this](hellfire::Application &app) { init(app); };
    application_callbacks.setup = [this](hellfire::SceneManager &sm, const hellfire::AppInfo &info,
                                         hellfire::ShaderManager &shader_manager) {
        setup(sm, info, shader_manager);
    };
    application_callbacks.update = [this](float dt) { update(dt); };
    application_callbacks.process_input = [this](hellfire::Application &app, float dt) {
        handle_input(app, dt);
    };
    application_callbacks.on_mouse_moved = [this](float x_offset, float y_offset) {
        process_mouse_movement(x_offset, y_offset);
    };

    app.set_callbacks(application_callbacks);
}

void Game::init(hellfire::Application &app) {

    
    
}

void Game::on_scene_activated(hellfire::Scene *scene) const {
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

    std::clog << "Activated scene: " << scene_name << "\n";
}

void Game::setup(hellfire::SceneManager &sm, hellfire::AppInfo window, hellfire::ShaderManager &shader_manager) {
    auto im = hellfire::ServiceLocator::get_service<hellfire::InputManager>();
    im->set_cursor_mode(hellfire::HIDDEN); 
    
    scene_manager_ = &sm;

    // Scene loading with code behind
    scenes_["BoatScene"] = load_sponza_scene(window);

    // Set the initial active scene
    sm.set_active_scene(scenes_["BoatScene"]);

    // Register scene activation callback with the scene manager
    sm.set_scene_activated_callback([this](hellfire::Scene *scene) {
        on_scene_activated(scene);
    });
}

void Game::handle_input(hellfire::Application &app, float delta_time) {
    auto input_manager = hellfire::ServiceLocator::get_service<hellfire::InputManager>();

    if (input_manager->is_key_pressed(GLFW_KEY_1)) {
        scene_manager_->set_active_scene(scenes_["BoatScene"]);
    }
    if (input_manager->is_key_pressed(GLFW_KEY_2)) {
        if (!scenes_["SolarSystem"]) // Lazy initialization
            scenes_["SolarSystem"] = load_solar_system_scene(app.get_window_info());
        
        scene_manager_->set_active_scene(scenes_["SolarSystem"]);
    }

    AnimationInputHandler::get_instance().handle_input();
}

void Game::process_mouse_movement(float x_offset, float y_offset) const {
    // Find the camera entity with PlayerController and forward mouse movement
    if (!scene_manager_ || scenes_.empty()) {
        return;
    }

    auto *active_scene = scene_manager_->get_active_scene();
    if (!active_scene) {
        return;
    }

    // Look for the "Main Camera" entity 
    if (const auto *camera_entity = active_scene->find_entity_by_name("Main Camera")) {
        if (auto *player_controller = camera_entity->get_component<PlayerController>()) {
            player_controller->handle_mouse_movement(x_offset, y_offset);
        }
    }
}

void Game::update(float delta_time) const {
    for (auto &scene: scenes_) {
        scene.second->update(delta_time);
    }
}
