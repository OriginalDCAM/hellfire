//
// Created by denzel on 08/12/2025.
//

#pragma once

#include "glm/detail/type_vec.hpp"
#include "glm/detail/type_vec3.hpp"
#include "hellfire/assets/AssetRegistry.h"

namespace hellfire {
    /**
     * @brief Represents a node in the imported model hierarchy
     */
    struct ImportedNode {
        std::string name;
        glm::vec3 position{0.0f};
        glm::vec3 rotation{0.0f};
        glm::vec3 scale{1.0f};

        /// Indices into ImportResult::meshes
        std::vector<size_t> mesh_indices;
        /// Indices into ImportResult::nodes
        std::vector<size_t> child_indices;
    };

    /**
     * @brief Represents an imported mesh with its material binding
     */
    struct ImportedMesh {
        AssetID mesh_asset; // Reference to .hfmesh file
        AssetID material_asset; // Reference to .hfmat file
        std::string name;
    };

    /**
     * @brief Complete result of importing a model file
     */
    struct ImportResult {
        bool success = false;
        std::string error_message;

        std::vector<ImportedNode> nodes;
        std::vector<ImportedMesh> meshes;
        size_t root_node_index = 0;

        // All assets created during import
        std::vector<AssetID> created_mesh_assets;
        std::vector<AssetID> created_material_assets;
        std::vector<AssetID> created_texture_assets;
    };
}
