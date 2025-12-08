//
// Created by denzel on 05/12/2025.
//

#pragma once
#include "UI/Panels/EditorPanel.h"

namespace hellfire::editor {
    class NewProjectPanel : EditorPanel {
    public:
        void render() override;

        void set_context(EditorContext *ctx) override;

        bool should_open_ = false;
    };
}
