//
// Created by denzel on 23/09/2025.
//
#include "EditorApplication.h"

#include "../EditorStyles.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "../ui/Panels/EditorPanel.h"
#include "hellfire/core/Application.h"
#include "hellfire/platform/IWindow.h"
#include "hellfire/utilities/ServiceLocator.h"
#include "hellfire/platform/windows_linux/GLFWWindow.h"
#include "../IconsFontAwesome6.h"
#include "../scenes/DefaultScene.h"
#include "events/StateEvents.h"
#include "../states/Editor/EditorState.h"
#include "../states/ProjectHub/ProjectHubState.h"
#include "states/ProjectCreator/ProjectCreatorState.h"
#include "states/ProjectLoading/ProjectLoadingState.h"

namespace hellfire::editor {
    void EditorApplication::on_initialize(Application &app) {
        app.get_window_info().should_warp_cursor = false;

        auto *window = ServiceLocator::get_service<IWindow>();
        if (!window) {
            throw std::runtime_error("EditorPlugin: No window service available");
        }

        // Make sure vsync is enabled by default
        // window->enable_vsync(true);

        initialize_imgui(window);

        state_manager_.register_state<ProjectHubState>();
        state_manager_.register_state<ProjectCreatorState>();
        state_manager_.register_state<ProjectLoadingState>();
        state_manager_.register_state<EditorState>();
        state_manager_.set_context(&editor_context_);
        editor_context_.project_manager = std::make_unique<ProjectManager>(editor_context_.event_bus, editor_context_);

        auto *pm = editor_context_.project_manager.get();
        // Subscribe to state transitions
        editor_context_.event_bus.subscribe<OpenProjectCreatorEvent>([this](const auto &) {
            state_manager_.switch_to<ProjectCreatorState>();
        });

        editor_context_.event_bus.subscribe<CancelProjectCreatorEvent>([this](const auto &) {
            state_manager_.switch_to<ProjectHubState>();
        });

        editor_context_.event_bus.subscribe<CreateProjectEvent>([this, pm](const CreateProjectEvent &e) {
            state_manager_.switch_to<ProjectLoadingState>();
            pm->create_project_async(e.name, e.location, e.template_id);
        });

        editor_context_.event_bus.subscribe<OpenProjectEvent>([this, pm](const OpenProjectEvent &e) {
            state_manager_.switch_to<ProjectLoadingState>();
            pm->open_project_async(e.path);
        });

        editor_context_.event_bus.subscribe<ProjectLoadCompleteEvent>([this](const auto &) {
            state_manager_.switch_to<EditorState>();
        });

        editor_context_.event_bus.subscribe<CloseProjectEvent>([this, pm](const auto &) {
            pm->close_project();
            state_manager_.switch_to<ProjectHubState>();
        });

        // Start with project selection state
        state_manager_.switch_to<ProjectHubState>();
    }

    void EditorApplication::load_editor_ui_config(ImGuiIO& io) {
        io.IniFilename = nullptr;
        
        if (std::filesystem::exists("imgui.ini")) {
            ImGui::LoadIniSettingsFromDisk("imgui.ini");
        } else {
            ImGui::LoadIniSettingsFromDisk("assets/layouts/default.ini");
        }
    }

    void EditorApplication::initialize_imgui(IWindow *window) {
        // Setup ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();

        load_editor_ui_config(io);

        // Load default font
        io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Regular.ttf", 16.0f);
        static constexpr ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.GlyphMinAdvanceX = 16.0f;
        icons_config.GlyphOffset = ImVec2(0, 0);

        io.Fonts->AddFontFromFileTTF("assets/fonts/Font Awesome 7 Free-Solid-900.otf", 16.0f, &icons_config,
                                     icons_ranges);

        // Enable docking and multi-viewport
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        io.ConfigViewportsNoDecoration = false;
        io.ConfigViewportsNoTaskBarIcon = false;
        io.ConfigViewportsNoAutoMerge = false;

        io.ConfigErrorRecoveryEnableAssert = true;

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
        ImGui_ImplOpenGL3_Init("#version 440");

        imgui_initialized_ = true;
    }

    EditorApplication::~EditorApplication() {
        cleanup_imgui();
    }

    void EditorApplication::cleanup_imgui() {
        if (imgui_initialized_) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            imgui_initialized_ = false;
        }
    }

    void EditorApplication::on_begin_frame() {
        if (!imgui_initialized_) return;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }

    void EditorApplication::on_end_frame() {
        if (!imgui_initialized_) return;

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Handle multi-viewport
        if (const ImGuiIO &io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            auto *window = ServiceLocator::get_service<IWindow>();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            window->make_current();
        }
    }

    void EditorApplication::sync_editor_context() {
        if (const auto sm = ServiceLocator::get_service<SceneManager>();
            sm && editor_context_.active_scene != sm->get_active_scene()) {
            editor_context_.active_scene = sm->get_active_scene();
        }
    }

    void EditorApplication::on_render() {
        if (!imgui_initialized_) return;
        editor_context_.process_main_thread_queue();

        // Sync editor context with scene manager
        sync_editor_context();

        state_manager_.render();
    }

    bool EditorApplication::on_key_down(int key) {
        return state_manager_.on_key_down(key);
    }

    bool EditorApplication::on_key_up(int key) {
        return state_manager_.on_key_up(key);
    }

    bool EditorApplication::on_mouse_button(int button, bool pressed) {
        return state_manager_.on_mouse_button(button, pressed);
    }

    bool EditorApplication::on_mouse_move(float x, float y, float x_offset, float y_offset) {
        return state_manager_.on_mouse_move(x, y, x_offset, y_offset);
    }

    bool EditorApplication::on_mouse_wheel(float delta) {
        return state_manager_.on_mouse_wheel(delta);
    }

    void EditorApplication::on_window_resize(int width, int height) {
        IApplicationPlugin::on_window_resize(width, height);
    }

    void EditorApplication::on_window_focus(bool focused) {
        IApplicationPlugin::on_window_focus(focused);
    }

    Entity *EditorApplication::get_render_camera_override() {
        return state_manager_.get_render_camera_override();
    }
}
