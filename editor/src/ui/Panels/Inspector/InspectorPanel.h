//
// Created by denzel on 14/10/2025.
//

#pragma once
#include "core/EditorAssetManager.h"
#include "hellfire/ecs/components/MeshComponent.h"
#include "ui/Panels/EditorPanel.h"

namespace hellfire {
    class LightComponent;
    class RenderableComponent;
}

namespace hellfire::editor {
    class InspectorPanel : public EditorPanel {
    public:
        InspectorPanel() {
            mesh_texture_ = ServiceLocator::get_service<EditorAssetManager>()->load_icon("assets/thumbnails/model_thumbnail.png");
        }
        
        void render_add_component_context_menu(Entity *selected_entity);

        void render() override;
        /// Transform specific properties, position (translation), rotation, scale
        static void render_transform_component(TransformComponent* transform);
        /// Mesh specific properties, primarily the mesh type
        void render_mesh_component(MeshComponent * mesh);

        void render_asset_tile(const AssetMetadata &asset, float size, MeshComponent &mesh_comp);

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
        
    private:
        Texture* mesh_texture_ = nullptr;
        AssetID selected_asset_ = -1;
        float thumbnail_size_ = 80.0f;
        bool should_open_asset_selector_ = false;

        static std::string truncate_string(const std::string & name, int i);

        void swap_mesh(const AssetMetadata &asset, MeshComponent &mesh_comp);
    };
}
