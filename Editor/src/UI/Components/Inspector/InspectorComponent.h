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
        void render_transform_component(TransformComponent* transform);
        void render_mesh_component(MeshComponent * mesh);
        void render_renderable_component(RenderableComponent* renderable);
        void render_light_component(LightComponent* light);
        void render_camera_component(CameraComponent* camera);
        void render_script_component(ScriptComponent* script);
            
    };
}
