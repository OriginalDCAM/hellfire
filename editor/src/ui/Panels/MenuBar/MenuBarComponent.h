//
// Created by denzel on 01/10/2025.
//

#pragma once
#include "../EditorPanel.h"

namespace hellfire::editor {
class MenuBarComponent : public EditorPanel {
public:
    MenuBarComponent(EditorContext* ctx) : EditorPanel(ctx) {}


    void render() override;
private:
    void render_file_menu();
    void render_scene_menu();
    void render_layout_menu();
    void handle_new_scene();
    void handle_open_scene() const;
    void render_scene_list() const;
};
}