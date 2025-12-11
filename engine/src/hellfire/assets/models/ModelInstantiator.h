//
// Created by denzel on 08/12/2025.
//

#pragma once
#include "ImportResult.h"
#include "hellfire/assets/AssetManager.h"
#include "hellfire/scene/Scene.h"

namespace hellfire {
    /**
     * @brief Creates scene entities from an ImportResult
     */
    class ModelInstantiator {
    public:
        ModelInstantiator(AssetManager& assets, AssetRegistry& registry);

        // Instantiate from AssetID (looks up .hfmodel file)
        EntityID instantiate(Scene& scene, AssetID model_asset_id, EntityID parent = INVALID_ENTITY);

        // Instantiate from path directly
        EntityID instantiate(Scene& scene, const std::filesystem::path& hfmodel_path, EntityID parent = INVALID_ENTITY);

        // Instantiate from already-loaded ImportResult
        EntityID instantiate(Scene& scene, const ImportResult& model, EntityID parent = INVALID_ENTITY);

    private:
        AssetManager& assets_;
        AssetRegistry& registry_;

        EntityID instantiate_node(Scene& scene, 
                                  const ImportResult& model,
                                  size_t node_index, 
                                  EntityID parent);

        void attach_mesh_components(Entity *entity, const ImportedMesh &mesh_ref);
    };


}
