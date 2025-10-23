//
// Created by denzel on 14/10/2025.
//

#pragma once
#include "hellfire/ecs/components/MeshComponent.h"
#include "UI/Panels/EditorPanel.h"

namespace hellfire {
    class LightComponent;
    class RenderableComponent;
}

namespace hellfire::editor {
    class InspectorPanel : public EditorPanel {
    public:

        void render() override;
        /// Transform specific properties, position (translation), rotation, scale
        static void render_transform_component(TransformComponent* transform);
        /// Mesh specific properties, primarily the mesh type
        static void render_mesh_component(MeshComponent * mesh);
        /// Material specific properties Color, Textures, shader uniforms
        static void render_renderable_component(RenderableComponent* renderable);
        /// Directional light specific properties, direction
        static void render_directional_light_component(LightComponent * light);
        /// Point light specific properties, attentuation, range, etc.
        static void render_point_light_component(LightComponent * light);
        /// General light properties, color, intensity, type
        static void render_light_component(LightComponent* light);
        /// Camera specific properties, camera type, clip planes, 
        static void render_camera_component(CameraComponent* camera);
        /// Script specific properties, for now just booleans
        static void render_script_component(const ScriptComponent* script);
            
    };
}
