//
// Created by denzel on 05/12/2025.
//

#pragma once
#include <filesystem>

#include "core/ApplicationState.h"
#include "Project/ProjectManager.h"

namespace hellfire::editor {
    
    class ProjectHubState : public ApplicationState {
    public:
        void on_enter() override;
        void render() override;

    private:
        void render_sidebar();
        void render_project_list();
        void render_buttons();

        std::vector<RecentProject> recent_projects_;
        int selected_tab_ = 0;
    };
}
