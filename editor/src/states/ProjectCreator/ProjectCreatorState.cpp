//
// Created by denzel on 05/12/2025.
//

#include "ProjectCreatorState.h"

#include "IconsFontAwesome6.h"
#include "imgui.h"
#include "events/StateEvents.h"
#include "ui/ui.h"
#include "ui/Panels/EditorPanel.h"

namespace hellfire::editor {
    void ProjectCreatorState::on_enter() {
        // Reset form
        memset(project_name_, 0, sizeof(project_name_));
        memset(project_location_, 0, sizeof(project_location_));
        selected_template_ = 0;

        templates_ = context_->project_manager->get_templates();
    }

    void ProjectCreatorState::render() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        if (ui::Window window{
            "Hellfire - Project Hub", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
        }) {
            // Center the form
            float form_width = 500.0f;
            float offset_x = (ImGui::GetContentRegionAvail().x - form_width) * 0.5f;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset_x);

            if (ui::ChildWindow child{"CreatorForm", ImVec2(form_width, 0)}) {
                ImGui::Dummy(ImVec2(0, 50)); // Top padding

                render_form();

                ImGui::Dummy(ImVec2(0, 20));
                render_buttons();
            }
        }
    }

    void ProjectCreatorState::render_form() {
        float label_width = 120.0f;
    
        // Project Name
        ImGui::Text("Project Name");
        ImGui::SameLine(label_width);
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##name", project_name_, sizeof(project_name_));
    
        ImGui::Dummy(ImVec2(0, 5));
    
        // Project Location
        ImGui::Text("Project Location");
        ImGui::SameLine(label_width);
        ImGui::SetNextItemWidth(-30);
        ImGui::InputText("##location", project_location_, sizeof(project_location_));
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_FOLDER)) {
            std::string folder = Utility::FileDialog::select_folder("Select Project Folder");
            if (!folder.empty()) {
                strncpy_s(project_location_, 256, folder.c_str(), 255);
            }
        }

        ImGui::Dummy(ImVec2(0, 5));

        if (strlen(project_name_) > 0 && strlen(project_location_) > 0) {
            ImGui::TextDisabled("Will create:");
            ImGui::SameLine(label_width);
            ImGui::TextDisabled("%s\\%s", project_location_, project_name_);
        }

        ImGui::Dummy(ImVec2(0, 5));
    
        // Template
        ImGui::Text("Project Template");
        ImGui::SameLine(label_width);
        ImGui::SetNextItemWidth(-1);
    
        if (ImGui::BeginListBox("##templates", ImVec2(-1, 80))) {
            for (int i = 0; i < templates_.size(); i++) {
                bool selected = (selected_template_ == i);
                if (ImGui::Selectable(templates_[i].name.c_str(), selected)) {
                    selected_template_ = i;
                }
            }
            ImGui::EndListBox();
        }
    }

    void ProjectCreatorState::render_buttons() {
        float button_width = 100.0f;
        float total_width = button_width * 2 + ImGui::GetStyle().ItemSpacing.x;
    
        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - total_width);
    
        bool can_create = strlen(project_name_) > 0 && strlen(project_location_) > 0;
    
        ImGui::BeginDisabled(!can_create);
        if (ImGui::Button("Create", ImVec2(button_width, 0))) {
            context_->event_bus.dispatch<CreateProjectEvent>(
                project_name_,
                std::filesystem::path(project_location_),
                templates_[selected_template_].id
            );
        }
        ImGui::EndDisabled();
    
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(button_width, 0))) {
            context_->event_bus.dispatch<CancelProjectCreatorEvent>();
        }
    }
}
