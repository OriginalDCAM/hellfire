//
// Created by denzel on 03/04/2025.
//

#ifndef GAME_H
#define GAME_H

#include <DCraft.h>

class Game {
public:
    Game() {
    }

    ~Game() {
        // Scenemanager handle deletion of Scenes
        // Scenes handle deletion of objects
    }

    void setup_callbacks(DCraft::Application &app);

    bool drone_key_processed = false;
    bool drone_mode_active = false;
    float drone_toggle_timer = 0.0f;
    const float DRONE_TOGGLE_COOLDOWN = 0.2f; // 200ms cooldown

    void init(DCraft::Application &app);


    void setup_animations();

    void setup(DCraft::SceneManager &sm, DCraft::WindowInfo window, DCraft::ShaderManager &shader_manager);


    // Input handler
    void handle_input(DCraft::Application &app, float delta_time);

    void process_mouse_movement(float x_offset, float y_offset);

    void update(float delta_time);
private:
    DCraft::SceneManager *scene_manager_;
    std::unordered_map<std::string, DCraft::Scene *> scenes_;
    std::string active_scene_name_ = "Sandbox";

    void on_scene_activated(DCraft::Scene *scene);

    void setup_cameras_for_scene(const std::string &scene_name);

    struct SceneCameras {
        DCraft::Entity *main_camera = nullptr;
        DCraft::Entity *drone_camera = nullptr;
        DCraft::Entity *main_camera_visual = nullptr;
        DCraft::Entity *drone_camera_visual = nullptr;
    };

    DCraft::AnimationSystem animation_system_;

    SceneCameras &get_active_scene_cameras();

    std::unordered_map<std::string, SceneCameras> scene_cameras_;
};


#endif //GAME_H
