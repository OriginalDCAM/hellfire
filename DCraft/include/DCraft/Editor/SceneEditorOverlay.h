//
// Created by denzel on 09/04/2025.
//

#pragma once
#include "imgui.h"
#include "Commands/RemoveObjectCommand.h"
#include "DCraft/Addons/SceneManager.h"
#include "DCraft/Graphics/Lights/Light.h"
#include "DCraft/Graphics/Primitives/MeshRenderer.h"

namespace DCraft {
    class Renderer;
    class Camera;
}

namespace DCraft::Editor {
    class SceneEditorOverlay {
    public:
        SceneEditorOverlay(Renderer* renderer, SceneManager& scene_manager)
            : renderer_(renderer), 
              scene_manager_(scene_manager),
              selected_node_(nullptr),
              active_scene_(nullptr), 
              active_camera_(nullptr), 
              viewport_focused_(false), 
              viewport_size_(800, 600),
              current_command_index_(-1),
              viewport_needs_update_(true),
              scene_changed_(true),
              cached_scene_texture_(0),
              last_camera_position_(0.0f),
              last_camera_rotation_(0.0f) {}

        void render();
        void render_object_node(Object3D *node);

        // Viewport management
        void set_active_scene(Object3D* scene) { active_scene_ = scene; }
        void set_active_camera(Camera* camera) { active_camera_ = camera; }
        bool is_viewport_focused() const { return viewport_focused_; }

        // Selection management
        Object3D* get_selected_object() const { return selected_node_; }
        void set_selected_object(Object3D* object) { selected_node_ = object; }

        void mark_scene_dirty();
        void mark_viewport_dirty();

    private:
        // Core components
        Renderer* renderer_;
        SceneManager& scene_manager_;
        
        // Selection and viewport state
        Object3D* selected_node_;
        Object3D* active_scene_;
        Camera* active_camera_;
        bool viewport_focused_;
        ImVec2 viewport_size_;

        // Command history for undo/redo
        std::vector<std::unique_ptr<EditorCommand>> command_history_;
        int current_command_index_;

        bool viewport_needs_update_;
        bool scene_changed_;
        uint32_t cached_scene_texture_;
        glm::vec3 last_camera_position_;
        glm::vec3 last_camera_rotation_;

        // Rendering methods
        void render_viewport();
        void render_scene_hierarchy();
        void render_object_properties(Object3D* object) const;
        void render_light_properties(Light* light) const;
        void render_camera_properties(Camera* camera) const;
        void render_shape_properties(MeshRenderer* shape) const;
        void render_mesh_properties(Mesh* mesh) const;

        // Input handling
        void handle_viewport_input();

        bool check_if_render_needed();

        // Command management
        void execute_command(std::unique_ptr<EditorCommand> command);
        void undo_last_command();
        void redo_next_command();
    };
}