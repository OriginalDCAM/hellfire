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

    void setup(hellfire::SceneManager &sm, const hellfire::AppInfo& window, hellfire::ShaderManager &shader_manager);


    // Input handler
    void handle_input(float delta_time);

    void process_mouse_movement(float x_offset, float y_offset) const;

    void update(float delta_time);
private:
    hellfire::SceneManager *scene_manager_;
    std::unordered_map<std::string, hellfire::Scene *> scenes_;
    const hellfire::AppInfo* app_info_ = nullptr;

    void on_scene_activated(hellfire::Scene *scene) const;
};


#endif //GAME_H
