//
// Created by denzel on 05/12/2025.
//

#include "EditorState.h"

#include "hellfire/graphics/renderer/Renderer.h"
#include "hellfire/scene/SceneManager.h"
#include "scenes/DefaultScene.h"
#include "ui/Panels/AssetExplorer/AssetExplorer.h"
#include "ui/Panels/Inspector/InspectorPanel.h"
#include "ui/Panels/MenuBar/MenuBarComponent.h"
#include "ui/Panels/SceneHierarchy/SceneHierarchyPanel.h"
#include "ui/Panels/Settings/Renderer/RendererSettingsPanel.h"
#include "ui/Panels/Viewport/SceneCameraScript.h"
#include "ui/Panels/Viewport/ViewportPanel.h"

namespace hellfire::editor {
    void EditorState::on_enter() {
        // Make sure the renderer render's the scene to a framebuffer
        ServiceLocator::get_service<Renderer>()->set_render_to_framebuffer(true);
        
        // Initialize and set context for UI components
        menu_bar_ = std::make_unique<MenuBarComponent>(context_);
        panel_manager_.add_panel<SceneHierarchyPanel>();
        panel_manager_.add_panel<InspectorPanel>();
        panel_manager_.add_panel<RendererSettingsPanel>();
        panel_manager_.add_panel<AssetExplorer>();
        viewport_panel_ = panel_manager_.add_panel<ViewportPanel>();
        
        panel_manager_.set_context(context_);
    }

    void EditorState::on_exit() {
        ApplicationState::on_exit();
        panel_manager_.remove_all_panels();
        
    }

    void EditorState::render() {
        // Create main dockspace
        create_dockspace();

        panel_manager_.render_all();
    }

    void EditorState::create_dockspace() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        const ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
                                        ImGuiWindowFlags_NoTitleBar |
                                        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                        ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        menu_bar_->render();
        // Create dockspace
        const ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

        ImGui::End();
    }

    bool EditorState::on_mouse_move(float x, float y, float x_offset, float y_offset) {
        if (viewport_panel_ && viewport_panel_->is_editor_camera_active()) {
            if (const auto* camera = viewport_panel_->get_editor_camera()) {
                if (auto* script = camera->get_component<SceneCameraScript>()) {
                    script->handle_mouse_movement(x_offset, y_offset);
                }
            }
            return true;
        }
    
        if (ImGui::GetIO().WantCaptureMouse) {
            return true;
        }
    
        return false;
    }

    bool EditorState::on_mouse_wheel(float delta) {
        if (ImGui::GetIO().WantCaptureMouse) {
            return true;
        }
    
        return false;
    }

    Entity* EditorState::get_render_camera_override() {
        return viewport_panel_ ? viewport_panel_->get_editor_camera() : nullptr;
    }
}
