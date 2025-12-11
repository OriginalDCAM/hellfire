//
// Created by denzel on 05/12/2025.
//

#include "NewProjectPanel.h"

#include "ui/ui.h"
#include "events/WindowEvents.h"

namespace hellfire::editor {
    void NewProjectPanel::render() {
        if (!should_open_) return;
        
        if (ui::Window window{"New Project"}) {
            
            ImGui::Text("Hello Test");
        }
    }

    void NewProjectPanel::set_context(EditorContext *ctx) {
        EditorPanel::set_context(ctx);

        context_->event_bus.subscribe<OpenNewProjectWindowEvent>([this](const OpenNewProjectWindowEvent) {
            this->should_open_ = true;
        });
    }
}
