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
        ModelInstantiator(Scene& scene, AssetManager& assets);

        EntityID instantiate(const ImportResult& result, EntityID parent = 0);

        // Instantiate a single mesh as an entity
        EntityID instantiate_mesh(AssetID mesh_asset, 
                         AssetID material_asset = INVALID_ASSET_ID,
                         EntityID parent = 0);

    private:
        Scene& scene_;
        AssetManager& assets_;

        EntityID instantiate_node(const ImportResult& result,
                         size_t node_index,
                         EntityID parent);
    };
}
