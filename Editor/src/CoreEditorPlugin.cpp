//
// Created by denzel on 23/09/2025.
//
#include "CoreEditorPlugin.h"

#include "CoreEditorStyles.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui.h"
#include "EditorComponent.h"
#include "MenuBarComponent.h"
#include "SceneHierarchyComponent.h"
#include "hellfire/core/Application.h"
#include "hellfire/platform/IWindow.h"
#include "hellfire/utilities/ServiceLocator.h"
#include "hellfire/platform/windows_linux/GLFWWindow.h"

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

        // Set context for UI components
        menu_bar_ = std::make_unique<MenuBarComponent>();
        menu_bar_->set_context(&editor_context_);

        scene_hierarchy_ = std::make_unique<SceneHierarchyComponent>();
        scene_hierarchy_->set_context(&editor_context_);
    }

    void CoreEditorPlugin::initialize_imgui(IWindow *window) {
        // Setup ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();

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

        GLFWwindow *glfw_window = static_cast<GLFWwindow *>(window->get_native_handle());
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
        auto sm = ServiceLocator::get_service<SceneManager>();
        if (sm && editor_context_.active_scene != sm->get_active_scene()) {
            editor_context_.active_scene = sm->get_active_scene();
        }

        // Create main dockspace
        create_dockspace();

        scene_hierarchy_->render();
        render_viewport_window();
    }

    void CoreEditorPlugin::render_viewport_stats_overlay() {
        ImVec2 window_pos = ImGui::GetWindowPos();

        // Position overlay in top-left corner with padding
        const float padding = 10.0f;
        ImGui::SetNextWindowPos(ImVec2(window_pos.x + padding, window_pos.y + padding + ImGui::GetFrameHeight()));
        ImGui::SetNextWindowBgAlpha(0.35f);

        ImGuiWindowFlags overlay_flags =
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoFocusOnAppearing |
                ImGuiWindowFlags_NoNav |
                ImGuiWindowFlags_NoMove;

        if (ImGui::Begin("##ViewportStats", nullptr, overlay_flags)) {
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("Frame Time: %.2f ms", 1000.0f / ImGui::GetIO().Framerate);

            if (editor_context_.active_scene) {
                ImGui::Separator();
                ImGui::Text("Entities: %zu", editor_context_.active_scene->get_entity_count());
            }
        }
        ImGui::End();
    }

    void TextCentered(const std::string &text) {
        auto viewport_size = ImGui::GetContentRegionAvail();
        auto text_width = ImGui::CalcTextSize(text.c_str()).x;
        ImGui::SetCursorPos(ImVec2((viewport_size.x - text_width) / 2, viewport_size.y / 2));
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text(text.c_str());
        ImGui::SetWindowFontScale(1.0f);
    }

    void CoreEditorPlugin::render_viewport_window() {
        if (!editor_context_.active_scene) return;

        const std::string &scene_name = editor_context_.active_scene->get_name();

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImVec2 default_size = ImVec2(viewport->Size.x / 1.5, viewport->Size.y / 1.5);
        ImVec2 default_pos = ImVec2(
            viewport->Pos.x + (viewport->Size.x - default_size.x) * 0.5f,
            viewport->Pos.y + (viewport->Size.y - default_size.y) * 0.5f
        );

        ImGui::SetNextWindowSize(default_size, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(default_pos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(320, 180), ImVec2(UINT_MAX, UINT_MAX));

        if (ImGui::Begin(scene_name.c_str())) {
            ImVec2 viewport_size = ImGui::GetContentRegionAvail();

            // Store last valid size
            static ImVec2 last_valid_size = {800, 600};
            static ImVec2 last_resize_size = {0, 0};
            static float last_resize_time = 0.0f;
            const float RESIZE_DELAY = 0.016f; 

            // Use last valid size if current one is invalid
            if (viewport_size.x > 0 && viewport_size.y > 0) {
                last_valid_size = viewport_size;
            } else {
                viewport_size = last_valid_size;
            }

            auto *renderer = ServiceLocator::get_service<Renderer>();
            float current_time = static_cast<float>(ImGui::GetTime());

            // Only resize if dimensions actually changed
            static ImVec2 last_viewport_size = {0, 0};
            if (viewport_size.x != last_viewport_size.x || viewport_size.y != last_viewport_size.y &&  (current_time - last_resize_time) > RESIZE_DELAY) {
                renderer->resize_scene_framebuffer(static_cast<uint32_t>(viewport_size.x),
                                                   static_cast<uint32_t>(viewport_size.y));
                last_viewport_size = viewport_size;
                last_resize_time = current_time;

                // Update camera's aspect ratio
                if (auto *camera = editor_context_.active_scene->get_active_camera()) {
                    const float aspect_ratio = viewport_size.x / viewport_size.y;
                    camera->set_aspect_ratio(aspect_ratio);
                }
            }

            const uint32_t scene_texture = renderer->get_scene_texture();
            if (!editor_context_.active_scene->get_active_camera()) {
                TextCentered("No Active Camera In Scene!");
            } else {
                ImGui::Image(scene_texture, viewport_size);
            }

            render_viewport_stats_overlay();
        }
        ImGui::End();
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
            return true; // ImGui consumed the input
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
}
