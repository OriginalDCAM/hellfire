//
// Created by denzel on 27/11/2025.
//

#include "RendererSettingsPanel.h"

#include "imgui.h"
#include "hellfire/graphics/renderer/Renderer.h"
#include "ui/ui.h"

namespace hellfire::editor {
    void RendererSettingsPanel::render() {
        if (ui::Window window{"Renderer Settings"}) {
            if (const auto renderer = ServiceLocator::get_service<Renderer>()) {
                ui::float_input("Shadow Bias", &renderer->get_shadow_settings().bias, 0.001);
            }
        }
    }
}
