//
// Created by denzel on 03/04/2025.
//

#ifndef GAME_H
#define GAME_H

#include <DCraft.h>

#include "Dcraft/Graphics/Primitives/Cube.h"


class Game {
public:
    Game() {}

    ~Game() {
        // Scenemanager handles deletion of objects for us
    }

    void setup_callbacks(DCraft::Application &app );

    bool drone_key_processed = false;
    bool drone_mode_active = false;
    float drone_toggle_timer = 0.0f;
    const float DRONE_TOGGLE_COOLDOWN = 0.2f; // 200ms cooldown

    void init(DCraft::Application &app);

    void setup(DCraft::SceneManager &sm, DCraft::WindowInfo window);

    // Input handler
    void handle_input(DCraft::Application &app, float delta_time);

    void process_mouse_movement(float x_offset, float y_offset);

    void process_camera_movement();

    void update(float delta_time);

private:
    DCraft::PerspectiveCamera *camera_ = nullptr;
    DCraft::PerspectiveCamera *drone_camera_ = nullptr;
    DCraft::Cube *cube_ = nullptr;
    
    DCraft::Cube *main_camera_visual_ = nullptr;
    DCraft::Cube *drone_camera_visual_ = nullptr;
    
    DCraft::Scene *initial_scene;
    DCraft::SceneManager *scene_manager_;
};


#endif //GAME_H
