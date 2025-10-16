//
// Created by denzel on 14/10/2025.
//

#pragma once
#include "hellfire/ecs/components/MeshComponent.h"
#include "UI/Components/EditorComponent.h"

namespace hellfire {
    class LightComponent;
    class RenderableComponent;
}

namespace hellfire::editor {
    class InspectorComponent : public EditorComponent {
    public:

        void render() override;
        /// Transform specific properties, position (translation), rotation, scale
        void render_transform_component(TransformComponent* transform);
        /// Mesh specific properties, primarily the mesh type
        void render_mesh_component(MeshComponent * mesh);
        /// Material specific properties Color, Textures, shader uniforms
        void render_renderable_component(RenderableComponent* renderable);
        /// Directional light specific properties, direction
        void render_directional_light_component(LightComponent * light);
        /// Point light specific properties, attentuation, range, etc.
        void render_point_light_component(LightComponent * light);
        /// General light properties, color, intensity, type
        void render_light_component(LightComponent* light);
        /// Camera specific properties, camera type, clip planes, 
        void render_camera_component(CameraComponent* camera);
        /// Script specific properties, for now just booleans
        void render_script_component(ScriptComponent* script);
            
    };
}
