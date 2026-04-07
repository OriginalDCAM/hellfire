//
// Created by denzel on 05/12/2025.
//

#include "ProjectLoadingState.h"

#include "imgui.h"
#include "events/StateEvents.h"
#include "ui/ui.h"
#include "ui/Panels/EditorPanel.h"

namespace hellfire::editor {
    void ProjectLoadingState::on_enter() {
        log_messages_.clear();
        progress_ = 0.0f;

        context_->event_bus.subscribe<ProjectLoadProgressEvent>([this](const ProjectLoadProgressEvent &e) {
            log_messages_.push_back(e.message);
            progress_ = e.progress;
        });
    }

    void ProjectLoadingState::render() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        if (ui::Window window{"Creating Project", nullptr,
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize}) {
        
            // Progress bar
            ImGui::ProgressBar(progress_, ImVec2(-1, 0));
        
            ImGui::Dummy(ImVec2(0, 10));
        
            // Log output
            if (ui::ChildWindow child{"LogOutput", ImVec2(-1, -1), ImGuiChildFlags_Border}) {
                for (const auto& msg : log_messages_) {
                    ImGui::TextUnformatted(msg.c_str());
                }
            
                // Auto-scroll
                if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                    ImGui::SetScrollHereY(1.0f);
                }
            }
                }
    }
}