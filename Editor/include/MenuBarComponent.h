//
// Created by denzel on 01/10/2025.
//

#pragma once
#include "EditorComponent.h"

namespace hellfire::editor {
class MenuBarComponent : public EditorComponent {
public:
    void render() override;
private:
    void render_file_menu();
    void render_scene_menu();
    void handle_new_scene();
    void handle_open_scene();
    void render_scene_list();
};
};