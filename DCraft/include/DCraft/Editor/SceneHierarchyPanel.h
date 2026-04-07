//
// Created by denzel on 09/04/2025.
//

#pragma once
#include "Commands/RemoveObjectCommand.h"
#include "DCraft/Addons/SceneManager.h"
#include "DCraft/Graphics/Lights/Light.h"
#include "DCraft/Graphics/Primitives/Shape3D.h"

namespace DCraft::Editor {
    class SceneHierarchyPanel {
    public:
        SceneHierarchyPanel(SceneManager& scene_manager, Object3D*& selected_node)
            : scene_manager_(scene_manager), selected_node_(selected_node) {}

        void render();

        void render_object_node(Object3D *node);

    private:
        SceneManager& scene_manager_;
        Object3D*& selected_node_;

        // Command history for undo/redo
        std::vector<std::unique_ptr<EditorCommand>> command_history_;
        int current_command_index_ = -1;

        void undo();

        void redo();

        void render_menu_bar();

        void render_light_properties(Light * light) const;
        void render_camera_properties(Camera * camera) const;
        void render_shape_properties(Shape3D * shape) const;
        void render_mesh_properties(Mesh * mesh) const;

        void render_object_properties(Object3D * object) const;
        
    };
}
