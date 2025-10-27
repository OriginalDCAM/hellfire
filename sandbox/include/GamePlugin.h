//
// Created by denzel on 03/04/2025.
//

#ifndef GAME_H
#define GAME_H

#include <hellfire-core.h>

class GamePlugin : public hellfire::IApplicationPlugin {
public:
    // Input handler
    void handle_input();

    void process_mouse_movement(float x_offset, float y_offset) const;

private:
    hellfire::SceneManager *scene_manager_;
    std::unordered_map<std::string, hellfire::Scene *> scenes_;
    const hellfire::AppInfo* app_info_ = nullptr;

    void on_scene_activated(hellfire::Scene *scene);

public:
    void on_initialize(hellfire::Application &app) override;

    void on_render() override;

    bool on_mouse_move(float x, float y, float x_offset, float y_offset) override;
};

#endif //GAME_H
