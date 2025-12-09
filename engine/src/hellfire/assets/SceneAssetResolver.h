//
// Created by denzel on 09/12/2025.
//

#pragma once

#include "hellfire/assets/AssetManager.h"
#include "hellfire/scene/Scene.h"
#include "hellfire/ecs/components/MeshComponent.h"
#include "hellfire/ecs/RenderableComponent.h"

    namespace hellfire {

        class SceneAssetResolver {
        public:
            explicit SceneAssetResolver(AssetManager& assets) : assets_(assets) {}

            void resolve(Scene& scene) {
                for (const auto id: scene.get_all_entities() | std::views::keys) {
                    Entity* entity = scene.get_entity(id);
                    if (!entity) continue;

                    resolve_entity(*entity);
                }
            }

        private:
            void resolve_entity(Entity& entity) {
                if (auto* mesh = entity.get_component<MeshComponent>()) {
                    AssetID id = mesh->get_mesh_asset();
                    if (id != INVALID_ASSET_ID) {
                        mesh->set_mesh(assets_.get_mesh(id));
                    }
                }

                if (auto* renderable = entity.get_component<RenderableComponent>()) {
                    AssetID id = renderable->get_material_asset();
                    if (id != INVALID_ASSET_ID) {
                        renderable->set_material(assets_.get_material(id));
                    }
                }
            }

            AssetManager& assets_;
        };

    }
