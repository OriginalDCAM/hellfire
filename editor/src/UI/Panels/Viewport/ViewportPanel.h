//
// Created by denzel on 13/10/2025.
//

#pragma once
#include <imgui.h>

#include "ImGuizmo.h"
#include "hellfire/graphics/backends/opengl/Framebuffer.h"
#include "UI/Panels/EditorPanel.h"

namespace hellfire::editor {
    /**
     * @brief Viewport panel for the scene editor with camera controls, gizmos, and entity picking 
     */
    class ViewportPanel final : public EditorPanel {
    public:
        ViewportPanel();

        Framebuffer *get_picking_fbo();

        ~ViewportPanel() override;

        /**
         * @brief Renders the viewport panel including scene view, gizmos, and overlays
         * Called by the editor plugin/main loop
         */
        void render() override;

        /**
         * @brief Renders the viewport statistics overlay (FPS, entity count, etc.)
         */
        void render_viewport_stats_overlay() const;

        /**
         * @brief Picks an entity at the given screen-space coordinates
         * @param mouse_x X coordinate in screen space
         * @param mouse_y Y coordinate in screen space
         * @return EntityID at the given postion, or 0 if none
         */
        uint32_t pick_object_at_mouse(int mouse_x, int mouse_y);

        /**
         * @brief Handles mouse-based entity selection using the picking framebuffer
         */
        void handle_object_picking();

        Entity *get_editor_camera() const { return editor_camera_; }
        bool is_editor_camera_active() const { return camera_active_; }

        ImVec2 get_viewport_pos() const { return viewport_pos_; }
        ImVec2 get_viewport_size() const { return viewport_size_; }
        bool is_viewport_hovered() const { return viewport_hovered_; }

    private:
        /**
         * @brief Creates and initializes the editor camera with default settings
         * Sets movement speed, sensitivity, and initial transform
         */
        void create_editor_camera();

        /**
         * @brief Destroys the editor camera and cleans up resources
         */
        void destroy_editor_camera();

        /**
         * @brief Updates editor camera based on user input (WASD movement, mouse look) by an attached script
         * Only active when right mouse button is held
         */
        void update_camera_control();

        /**
         * @brief Renders the scene color buffer from the engine renderer
         */
        void render_viewport_image();

        /**
         * @brief Renders the transform gizmo for the selected entity using ImGuizmo
         */
        void render_transform_gizmo();


        // Camera State
        Entity *editor_camera_ = nullptr;
        bool camera_active_ = false; // True when right mouse button is held


        // Viewport state
        ImVec2 viewport_pos_;               // Top-left corner in screen space
        ImVec2 viewport_size_;              // Current viewport dimensions
        bool viewport_hovered_ = false;
        float last_resize_time_ = 0.0f;     // Used to debounce framebuffer resizing

        // Gizmo state
        ImGuizmo::OPERATION current_operation_ = ImGuizmo::TRANSLATE; 
        ImGuizmo::MODE current_mode_ = ImGuizmo::LOCAL; 
        bool is_using_gizmo_ = false; 

        // Rendering resources (not owned)
        Renderer *engine_renderer_ = nullptr;

        // Entity picking
        std::unique_ptr<Framebuffer> picking_fbo_; // Renders entity IDs for mouse picking
    };
}
