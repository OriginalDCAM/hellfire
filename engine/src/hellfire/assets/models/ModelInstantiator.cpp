//
// Created by denzel on 08/12/2025.
//

#include "ModelInstantiator.h"

#include "hellfire/ecs/RenderableComponent.h"
#include "hellfire/ecs/TransformComponent.h"
#include "hellfire/ecs/components/MeshComponent.h"
#include "hellfire/serializers/ModelSerializer.h"

namespace hellfire {
    ModelInstantiator::ModelInstantiator(AssetManager& assets, AssetRegistry& registry)
        : assets_(assets)
        , registry_(registry) 
    {}

    EntityID ModelInstantiator::instantiate(Scene& scene, AssetID model_asset_id, EntityID parent) {
        auto meta = registry_.get_asset(model_asset_id);
        if (!meta) {
            std::cerr << "ModelInstantiator: Asset not found: " << model_asset_id << std::endl;
            return INVALID_ENTITY;
        }

        return instantiate(scene, registry_.get_absolute_path(model_asset_id), parent);
    }

    EntityID ModelInstantiator::instantiate(Scene& scene, 
                                            const std::filesystem::path& hfmodel_path, 
                                            EntityID parent) {
        auto model_opt = ModelSerializer::load(hfmodel_path);
        if (!model_opt) {
            std::cerr << "ModelInstantiator: Failed to load model: " << hfmodel_path << std::endl;
            return INVALID_ENTITY;
        }

        return instantiate(scene, *model_opt, parent);
    }

    EntityID ModelInstantiator::instantiate(Scene& scene, 
                                            const ImportResult& model, 
                                            EntityID parent) {
        if (!model.success || model.nodes.empty()) {
            std::cerr << "ModelInstantiator: Invalid model data" << std::endl;
            return INVALID_ENTITY;
        }

        return instantiate_node(scene, model, model.root_node_index, parent);
    }

    EntityID ModelInstantiator::instantiate_node(Scene& scene,
                                                  const ImportResult& model,
                                                  size_t node_index,
                                                  EntityID parent) {
        const auto& node = model.nodes[node_index];

        // Create entity
        EntityID entity_id = scene.create_entity(node.name);
        Entity* entity = scene.get_entity(entity_id);

        // Set transform
        auto* transform = entity->get_component<TransformComponent>();
        if (transform) {
            transform->set_position(node.position);
            transform->set_rotation(node.rotation);
            transform->set_scale(node.scale);
        }

        // Set parent
        if (parent != INVALID_ENTITY) {
            scene.set_parent(entity_id, parent);
        }

        // Attach mesh components
        if (node.mesh_indices.size() == 1) {
            // Single mesh - attach directly to this entity
            const auto& mesh_ref = model.meshes[node.mesh_indices[0]];
            attach_mesh_components(entity, mesh_ref);
        } 
        else if (node.mesh_indices.size() > 1) {
            // Multiple meshes - create child entities for each
            for (size_t mesh_idx : node.mesh_indices) {
                const auto& mesh_ref = model.meshes[mesh_idx];

                EntityID mesh_entity_id = scene.create_entity(mesh_ref.name);
                Entity* mesh_entity = scene.get_entity(mesh_entity_id);
                scene.set_parent(mesh_entity_id, entity_id);

                attach_mesh_components(mesh_entity, mesh_ref);
            }
        }

        // Recursively instantiate children
        for (size_t child_idx : node.child_indices) {
            instantiate_node(scene, model, child_idx, entity_id);
        }

        return entity_id;
    }

    void ModelInstantiator::attach_mesh_components(Entity* entity, const ImportedMesh& mesh_ref) {
        // Add MeshComponent
        auto* mesh_comp = entity->add_component<MeshComponent>();
        mesh_comp->set_mesh_asset(mesh_ref.mesh_asset);

        // Load actual mesh data
        if (auto mesh = assets_.get_mesh(mesh_ref.mesh_asset)) {
            mesh_comp->set_mesh(mesh);
        }

        // Add RenderableComponent with material
        auto* renderable = entity->add_component<RenderableComponent>();
        renderable->set_material_asset(mesh_ref.material_asset);

        // Load actual material data
        if (mesh_ref.material_asset != INVALID_ASSET_ID) {
            if (auto material = assets_.get_material(mesh_ref.material_asset)) {
                renderable->set_material(material);
            }
        }
    }
    

}
