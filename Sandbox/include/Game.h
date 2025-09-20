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

    void setup_callbacks(hellfire::Application &app);

    void init(hellfire::Application &app);

    void setup(hellfire::SceneManager &sm, hellfire::AppInfo window, hellfire::ShaderManager &shader_manager);


    // Input handler
    void handle_input(hellfire::Application &app, float delta_time);

    void process_mouse_movement(float x_offset, float y_offset) const;

    void update(float delta_time) const;
private:
    hellfire::SceneManager *scene_manager_;
    std::unordered_map<std::string, hellfire::Scene *> scenes_;

    void on_scene_activated(hellfire::Scene *scene) const;
};


#endif //GAME_H
