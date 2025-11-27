//
// Created by denzel on 27/11/2025.
//

#include "RendererSettingsPanel.h"

#include "imgui.h"
#include "hellfire/graphics/renderer/Renderer.h"
#include "UI/ui.h"

namespace hellfire::editor {
void RendererSettingsPanel::render() {
    if (ImGui::Begin("Renderer Settings")) {
        if (auto renderer = ServiceLocator::get_service<Renderer>()) {
            ui::float_input("Shadow Bias", &renderer->get_shadow_settings().bias, 0.001);
        }
        ImGui::End();
    }
        
}
}
    
