//
// Created by denzel on 03/04/2025.
//

#ifndef GAME_H
#define GAME_H

#include <DCraft.h>

class Game {
public:
    Game() = default;
    ~Game() = default;

    void setup_callbacks(DCraft::Application &app);

    void init(DCraft::Application &app);

    void setup(DCraft::SceneManager &sm, DCraft::WindowInfo window, DCraft::ShaderManager &shader_manager);


    // Input handler
    void handle_input(DCraft::Application &app, float delta_time);

    void process_mouse_movement(float x_offset, float y_offset) const;

    void update(float delta_time) const;
private:
    DCraft::SceneManager *scene_manager_;
    std::unordered_map<std::string, DCraft::Scene *> scenes_;
    std::string active_scene_name_ = "Sandbox";

    void on_scene_activated(DCraft::Scene *scene);
};


#endif //GAME_H
