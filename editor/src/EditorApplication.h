//
// Created by denzel on 23/09/2025.
//

#pragma once
#include <memory>

#include "UI/Panels/EditorPanel.h"
#include "UI/Panels/MenuBar/MenuBarComponent.h"
#include "UI/Panels/SceneHierarchy/SceneHierarchyPanel.h"
#include "hellfire/Interfaces/IApplicationPlugin.h"
#include "hellfire/platform/IWindow.h"
#include "UI/Panels/Inspector/InspectorPanel.h"
#include "UI/Panels/Settings/Renderer/RendererSettingsPanel.h"
#include "UI/Panels/Viewport/ViewportPanel.h"

namespace hellfire::editor {
    class EditorApplication final : public IApplicationPlugin {
    public:
        void on_initialize(Application &app) override;

        void initialize_imgui(IWindow *window);

        ~EditorApplication() override;

        void cleanup_imgui();

        void on_begin_frame() override;

        void on_end_frame() override;

        void sync_editor_context();

        void on_render() override;

        void create_dockspace();

        bool on_key_down(int key) override;

        bool on_key_up(int key) override;

        bool on_mouse_button(int button, bool pressed) override;

        bool on_mouse_move(float x, float y, float x_offset, float y_offset) override;

        bool on_mouse_wheel(float delta) override;

        void on_window_resize(int width, int height) override;

        void on_window_focus(bool focused) override;

        Entity * get_render_camera_override() override;

    private:
        EditorContext editor_context_;
        bool imgui_initialized_ = false;
        
        // UI Components
        std::unique_ptr<MenuBarComponent> menu_bar_;
        std::unique_ptr<SceneHierarchyPanel> scene_hierarchy_;
        std::unique_ptr<ViewportPanel> scene_viewport_;
        std::unique_ptr<InspectorPanel> inspector_panel_;
        std::unique_ptr<RendererSettingsPanel> renderer_settings_panel_;
    };
}
