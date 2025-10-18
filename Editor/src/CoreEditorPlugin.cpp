//
// Created by denzel on 23/09/2025.
//
#include "CoreEditorPlugin.h"

#include "CoreEditorStyles.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui.h"
#include "UI/Components/EditorComponent.h"
#include "UI/Components/MenuBarComponent.h"
#include "UI/Components/SceneHierarchyComponent.h"
#include "hellfire/core/Application.h"
#include "hellfire/platform/IWindow.h"
#include "hellfire/utilities/ServiceLocator.h"
#include "hellfire/platform/windows_linux/GLFWWindow.h"
#include "UI/Components/Viewport/SceneCameraScript.h"
#include "UI/Components/Viewport/ViewportComponent.h"
#include "IconsFontAwesome6.h"
#include "Scenes/DefaultScene.h"

namespace hellfire::editor {
    void CoreEditorPlugin::on_initialize(Application &app) {
        app_ = &app;
        app_->get_window_info().should_warp_cursor = false;

        auto *window = ServiceLocator::get_service<IWindow>();
        if (!window) {
            throw std::runtime_error("EditorPlugin: No window service available");
        }

        // Make sure vsync is enabled by default
        window->enable_vsync(true);

        initialize_imgui(window);

        // Make sure the renderer render's the scene to a framebuffer
        ServiceLocator::get_service<Renderer>()->set_render_to_framebuffer(true);

        // Initialize and set context for UI components
        menu_bar_ = std::make_unique<MenuBarComponent>();
        menu_bar_->set_context(&editor_context_);

        scene_hierarchy_ = std::make_unique<SceneHierarchyComponent>();
        scene_hierarchy_->set_context(&editor_context_);

        scene_viewport_ = std::make_unique<ViewportComponent>();
        scene_viewport_->set_context(&editor_context_);

        inspector_panel_ = std::make_unique<InspectorComponent>();
        inspector_panel_->set_context(&editor_context_);

        // TEMP:
    const auto sm = hellfire::ServiceLocator::get_service<hellfire::SceneManager>();
        const auto new_scene = sm->create_scene("Test");
        setup_default_scene_with_default_entities(new_scene);
        sm->set_active_scene(new_scene);
        
    }

    void CoreEditorPlugin::initialize_imgui(IWindow *window) {
        // Setup ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();

        // Load default font
        io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Regular.ttf", 16.0f);
        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.GlyphMinAdvanceX = 16.0f;
        icons_config.GlyphOffset = ImVec2(0, 0);

        io.Fonts->AddFontFromFileTTF("assets/fonts/Font Awesome 7 Free-Solid-900.otf", 16.0f, &icons_config, icons_ranges);

        // Enable docking and multi-viewport
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        io.ConfigViewportsNoDecoration = false;
        io.ConfigViewportsNoTaskBarIcon = false;
        io.ConfigViewportsNoAutoMerge = false;

        // Setup style
        styles::SetupDarkRedModeStyle();

        ImGuiStyle &style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        auto *glfw_window = static_cast<GLFWwindow *>(window->get_native_handle());
        if (!glfw_window) {
            throw std::runtime_error("EditorPlugin: Could not get GLFW window handle");
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        imgui_initialized_ = true;
    }

    CoreEditorPlugin::~CoreEditorPlugin() {
        cleanup_imgui();
    }

    void CoreEditorPlugin::cleanup_imgui() {
        if (imgui_initialized_) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            imgui_initialized_ = false;
        }
    }

    void CoreEditorPlugin::on_begin_frame() {
        if (!imgui_initialized_) return;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void CoreEditorPlugin::on_end_frame() {
        if (!imgui_initialized_) return;

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Handle multi-viewport
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void CoreEditorPlugin::on_render() {
        if (!imgui_initialized_) return;

        // Sync editor context with scene manager
        const auto sm = ServiceLocator::get_service<SceneManager>();
        if (sm && editor_context_.active_scene != sm->get_active_scene()) {
            editor_context_.active_scene = sm->get_active_scene();
        }

        // Create main dockspace
        create_dockspace();

        if (show_demo_) {
            ImGui::ShowDemoWindow(&show_demo_);
        }

        scene_hierarchy_->render();
        inspector_panel_->render();
        scene_viewport_->render();
    }

    void CoreEditorPlugin::create_dockspace() {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
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
        ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

        ImGui::End();
    }

    bool CoreEditorPlugin::on_key_down(int key) {
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureKeyboard) {
            return true; // ImGui consumed the input
        }

        return false;
    }

    bool CoreEditorPlugin::on_key_up(int key) {
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureKeyboard) {
            return true;
        }

        return false;
    }

    bool CoreEditorPlugin::on_mouse_button(int button, bool pressed) {
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse) {
            return true;
        }

        return false;
    }

    bool CoreEditorPlugin::on_mouse_move(float x, float y, float x_offset, float y_offset) {
        if (scene_viewport_->is_editor_camera_active()) {
            // Update camera with offset
            scene_viewport_->get_editor_camera()
                ->get_component<SceneCameraScript>()
                ->handle_mouse_movement(x_offset, y_offset);
        
        return true;
        }
 

        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse) {
            return true;
        }

        return false;
    }

    bool CoreEditorPlugin::on_mouse_wheel(float delta) {
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse) {
            return true;
        }

        return false;
    }

    void CoreEditorPlugin::on_window_resize(int width, int height) {
        IApplicationPlugin::on_window_resize(width, height);
    }

    void CoreEditorPlugin::on_window_focus(bool focused) {
        IApplicationPlugin::on_window_focus(focused);
    }

    Entity *CoreEditorPlugin::get_render_camera_override() {
        return scene_viewport_->get_editor_camera();
    }
}
