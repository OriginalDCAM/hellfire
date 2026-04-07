//
// Created by denzel on 08/12/2025.
//

#include "ModelInstantiator.h"

#include "hellfire/ecs/RenderableComponent.h"
#include "hellfire/ecs/TransformComponent.h"
#include "hellfire/ecs/components/MeshComponent.h"

namespace hellfire {
    ModelInstantiator::ModelInstantiator(Scene &scene, AssetManager &assets) : scene_(scene), assets_(assets) {}

    EntityID ModelInstantiator::instantiate(const ImportResult &result, const EntityID parent) {
        if (!result.success || result.nodes.empty()) {
            return 0;
        }

        return instantiate_node(result, result.root_node_index, parent);
    }

    EntityID ModelInstantiator::instantiate_mesh(AssetID mesh_asset, AssetID material_asset, EntityID parent) {
        auto mesh = assets_.get_mesh(mesh_asset);
        if (!mesh) return 0;

        EntityID entity_id = scene_.create_entity("Mesh");
        Entity* entity = scene_.get_entity(entity_id);

        if (parent != 0) {
            scene_.set_parent(entity_id, parent);
        }

        auto* mesh_comp = entity->add_component<MeshComponent>();
        mesh_comp->set_mesh(mesh);

        if (material_asset != INVALID_ASSET_ID) {
            auto material = assets_.get_material(material_asset);
            if (material) {
                auto* renderable = entity->add_component<RenderableComponent>();
                renderable->set_material(material);
            }
        }

        return entity_id;
    }

    EntityID ModelInstantiator::instantiate_node(const ImportResult &result, size_t node_index, EntityID parent) {
                const auto& node = result.nodes[node_index];

        EntityID entity_id = scene_.create_entity(node.name);
        Entity* entity = scene_.get_entity(entity_id);

        // Set transform
        entity->transform()->set_position(node.position);
        entity->transform()->set_rotation(node.rotation);
        entity->transform()->set_scale(node.scale);

        if (parent != 0) {
            scene_.set_parent(entity_id, parent);
        }

        // Attach meshes
        if (node.mesh_indices.size() == 1) {
            // Single mesh: attach directly
            const auto& mesh_data = result.meshes[node.mesh_indices[0]];
            
            auto* mesh_comp = entity->add_component<MeshComponent>();
            mesh_comp->set_mesh(assets_.get_mesh(mesh_data.mesh_asset));

            if (mesh_data.material_asset != INVALID_ASSET_ID) {
                auto* renderable = entity->add_component<RenderableComponent>();
                renderable->set_material(assets_.get_material(mesh_data.material_asset));
            }
        } else if (node.mesh_indices.size() > 1) {
            // Multiple meshes: create child entities
            for (size_t mesh_idx : node.mesh_indices) {
                const auto& mesh_data = result.meshes[mesh_idx];
                
                EntityID mesh_entity = scene_.create_entity(mesh_data.name);
                scene_.set_parent(mesh_entity, entity_id);
                
                Entity* mesh_ent = scene_.get_entity(mesh_entity);
                
                auto* mesh_comp = mesh_ent->add_component<MeshComponent>();
                mesh_comp->set_mesh(assets_.get_mesh(mesh_data.mesh_asset));

                if (mesh_data.material_asset != INVALID_ASSET_ID) {
                    auto* renderable = mesh_ent->add_component<RenderableComponent>();
                    renderable->set_material(assets_.get_material(mesh_data.material_asset));
                }
            }
        }

        // Instantiate children
        for (size_t child_idx : node.child_indices) {
            instantiate_node(result, child_idx, entity_id);
        }

        return entity_id;
    }
}
