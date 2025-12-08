//
// Created by denzel on 05/12/2025.
//

#pragma once
#include "core/ApplicationState.h"
#include "project/ProjectManager.h"

namespace hellfire::editor {
    class ProjectCreatorState : public ApplicationState {
    public:
        void on_enter() override;
        void render() override;

    private:
        void render_form();
        void render_buttons();

        char project_name_[256] = "";
        char project_location_[512] = "";
        std::vector<ProjectTemplate> templates_;
        int selected_template_ = 0;
    };
}
