//
// Created by denzel on 08/12/2025.
//

#pragma once
#include "AssetRegistry.h"
#include "hellfire/graphics/Mesh.h"

namespace hellfire {
    class AssetManager {
    public:
        explicit AssetManager(AssetRegistry& registry);

        // Typed asset loading with caching
        std::shared_ptr<Mesh> get_mesh(AssetID id);
        std::shared_ptr<Material> get_material(AssetID id);
        std::shared_ptr<Texture> get_texture(AssetID id);

        // Cache management
        void unload(AssetID id);
        void clear_cache();
        void reload_modified(); // Reload assets that changed on disk

        // Stats
        size_t get_loaded_mesh_count() const { return mesh_cache_.size(); }
        size_t get_loaded_material_count() const { return material_cache_.size(); }
        size_t get_loaded_texture_count() const { return texture_cache_.size(); }

    private:
        AssetRegistry& registry_;

        std::unordered_map<AssetID, std::shared_ptr<Mesh>> mesh_cache_;
        std::unordered_map<AssetID, std::shared_ptr<Material>> material_cache_;
        std::unordered_map<AssetID, std::shared_ptr<Texture>> texture_cache_;
    };
} // hellfire