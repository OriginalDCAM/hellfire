// //
// // Created by denzel on 03/04/2025.
// //
//
#include "GamePlugin.h"

#include "GLFW/glfw3.h"
#include "Scenes/SolarSystemScene.h"
#include "Scenes/SponzaScene.h"
#include "Scripts/PlayerController.h"
#include "Utils/AnimationInputHandler.h"

void GamePlugin::on_initialize(hellfire::Application &app) {
    auto im = hellfire::ServiceLocator::get_service<hellfire::InputManager>();
    im->set_cursor_mode(hellfire::HIDDEN);
    
    app_info_ = &app.get_window_info();
    scene_manager_ = hellfire::ServiceLocator::get_service<hellfire::SceneManager>();
    
    // Scene loading with code behind
    scenes_["BoatScene"] = load_sponza_scene(*app_info_);
    scene_manager_->set_active_scene(scenes_["BoatScene"]);
    
}

void GamePlugin::on_render() {
    handle_input();
}

bool GamePlugin::on_mouse_move(float x, float y, float x_offset, float y_offset) {
    process_mouse_movement(x_offset, y_offset);
    return false; // consumed
}

void GamePlugin::handle_input() {
    auto input_manager = hellfire::ServiceLocator::get_service<hellfire::InputManager>();

    if (input_manager->is_key_pressed(GLFW_KEY_1)) {
        scene_manager_->set_active_scene(scenes_["BoatScene"]);
    }
    if (input_manager->is_key_pressed(GLFW_KEY_2)) {
        if (!scenes_["SolarSystem"]) // Lazy initialization
            scenes_["SolarSystem"] = load_solar_system_scene(*app_info_);

        scene_manager_->set_active_scene(scenes_["SolarSystem"]);
    }

    AnimationInputHandler::get_instance().handle_input();
}
void GamePlugin::process_mouse_movement(float x_offset, float y_offset) const {
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
        if (auto *player_controller = camera_entity->get_component<SceneCameraScript>()) {
            player_controller->handle_mouse_movement(x_offset, y_offset);
        }
    }
}
