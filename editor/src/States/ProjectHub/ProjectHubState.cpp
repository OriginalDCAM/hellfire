//
// Created by denzel on 05/12/2025.
//

#include "ProjectHubState.h"

#include "IconsFontAwesome6.h"
#include "imgui.h"
#include "Events/StateEvents.h"
#include "UI/ui.h"
#include "UI/Panels/EditorPanel.h"

namespace hellfire::editor {
    void ProjectHubState::on_enter() {
        recent_projects_ = context_->project_manager->get_recent_projects();
    }

    void ProjectHubState::render() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize;
        
        if (ui::Window window{"Hellfire - Project Hub", nullptr, flags}) {
            if (ImGui::BeginTable("HubLayout", 2, ImGuiTableFlags_Resizable)) {
                ImGui::TableSetupColumn("Sidebar", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("Content", ImGuiTableColumnFlags_WidthStretch);
            
                ImGui::TableNextRow();
            
                // Sidebar
                ImGui::TableNextColumn();
                render_sidebar();
            
                // Content
                ImGui::TableNextColumn();
                render_buttons();
                ImGui::Separator();
                render_project_list();
            
                ImGui::EndTable();
            } 
        }
    }

    void ProjectHubState::render_sidebar() {
        if (ImGui::Selectable("Projects", selected_tab_ == 0)) {
            selected_tab_ = 0;
        }
        if (ImGui::Selectable("Documentation", selected_tab_ == 1)) {
            selected_tab_ = 1;
        }
    }

    void ProjectHubState::render_project_list() {
        if (ui::ChildWindow child{"ProjectList"}) {
            for (const auto& project : recent_projects_) {
                ImGui::PushID(&project);
            
                bool selected = false;
                if (ImGui::Selectable("##project", &selected, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(0, 30))) {
                    if (ImGui::IsMouseDoubleClicked(0)) {
                        context_->event_bus.dispatch<OpenProjectEvent>(project.path);
                    }
                }
            
                ImGui::SameLine();
                ImGui::Text("%s", project.name.c_str());
                ImGui::SameLine(120);
                ImGui::Text("%s", project.path.string().c_str());

                if (ImGui::BeginPopupContextWindow(("##" + project.path.string()).c_str())) {
                    if (ImGui::MenuItem("Remove from recent projects...")) {
                        context_->project_manager->remove_from_recent(project.path);
                        recent_projects_ = context_->project_manager->get_recent_projects();
                    }
                    ImGui::EndPopup();
                }
            
                ImGui::PopID();
            }
        }
    }

    void ProjectHubState::render_buttons() {
        float button_width = 120.0f;
        float spacing = ImGui::GetContentRegionAvail().x - (button_width * 2) - ImGui::GetStyle().ItemSpacing.x;
        
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + spacing);
    
        if (ImGui::Button("New Project", ImVec2(button_width, 0))) {
            context_->event_bus.dispatch<OpenProjectCreatorEvent>();
        }
        ImGui::SameLine();
        if (ImGui::Button("Open Project", ImVec2(button_width, 0))) {
            // Open file dialog, then emit OpenProjectEvent
            hellfire::Utility::FileFilter project_ext_filter = {"Hellfire Project", "*.hfproj"};
            const std::string path_to_proj = Utility::FileDialog::open_file({project_ext_filter});
    
            context_->event_bus.dispatch<OpenProjectEvent>(path_to_proj);
        }
    }
}
