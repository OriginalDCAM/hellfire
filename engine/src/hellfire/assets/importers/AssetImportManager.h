//
// Created by denzel on 09/12/2025.
//

#pragma once
#include "hellfire/assets/AssetManager.h"
#include "hellfire/assets/AssetRegistry.h"

namespace hellfire {
class AssetImportManager {
  public:
   AssetImportManager(AssetRegistry& registry, AssetManager& asset_manager, const std::filesystem::path& project_root);

   /**
    * @brief Import all unprocessed assets in registry
    */
   void import_all_pending();

    void import_all_models();
    void import_all_textures();
    bool import_asset(AssetID id);

    // Check if asset needs import (no corresponding .hfmesh/.hfmat exists)
    bool needs_import(AssetID id) const;

    // Get import output directory
    std::filesystem::path get_import_output_dir() const { return import_output_dir_; };
private:
    AssetRegistry& registry_;
    AssetManager& asset_manager_;
    std::filesystem::path project_root_;
    std::filesystem::path import_output_dir_;

    bool import_model(const AssetMetadata& meta);
    bool import_texture(const AssetMetadata& meta);

    // Check if imported version exists
    bool has_imported_mesh(AssetID original_id) const;
    std::filesystem::path get_imported_path(const AssetMetadata& meta, 
                                             const std::string& extension) const;
};

} // hellfire

