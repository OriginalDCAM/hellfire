//
// Created by denzel on 09/12/2025.
//

#include "AssetImportManager.h"

#include "hellfire/assets/models/ModelImporter.h"
#include "hellfire/serializers/ModelSerializer.h"
#include "hellfire/serializers/TextureSerializer.h"

namespace hellfire {
    TextureType infer_texture_type(const std::string& name) {
        std::string lower_name = name;
        std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);

        if (lower_name.find("normal") != std::string::npos ||
            lower_name.find("nrm") != std::string::npos ||
            lower_name.find("_n.") != std::string::npos) {
            return TextureType::NORMAL;
            }
        if (lower_name.find("rough") != std::string::npos) {
            return TextureType::ROUGHNESS;
        }
        if (lower_name.find("metal") != std::string::npos) {
            return TextureType::METALNESS;
        }
        if (lower_name.find("ao") != std::string::npos ||
            lower_name.find("occlusion") != std::string::npos) {
            return TextureType::AMBIENT_OCCLUSION;
            }
        if (lower_name.find("emissive") != std::string::npos ||
            lower_name.find("emission") != std::string::npos) {
            return TextureType::EMISSIVE;
            }
        if (lower_name.find("spec") != std::string::npos) {
            return TextureType::SPECULAR;
        }

        return TextureType::DIFFUSE;  // Default
    }
    
    AssetImportManager::AssetImportManager(AssetRegistry &registry, AssetManager &asset_manager,
                                           const std::filesystem::path &project_root) : registry_(registry),
        asset_manager_(asset_manager), project_root_(project_root),
        import_output_dir_(project_root / "assets" / "imported") {
        create_directory(import_output_dir_);
    }

    void AssetImportManager::import_all_pending() {
        std::cout << "=== Scanning for assets to import ===" << std::endl;

        import_all_models();
        import_all_textures();

        std::cout << "=== Import complete ===" << std::endl;
    }

    void AssetImportManager::import_all_models() {
        auto models = registry_.get_assets_by_type(AssetType::MODEL);

        for (const auto &meta: models) {
            // Skip already-imported internal formats
            std::string ext = meta.filepath.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

            if (ext == ".hfmodel" || ext == ".hfmesh") {
                continue; // Already internal format
            }

            if (needs_import(meta.uuid)) {
                std::cout << "Importing model: " << meta.name << std::endl;
                import_model(meta);
            } else {
                std::cout << "Skipping (already imported): " << meta.name << std::endl;
            }
        }
    }

    void AssetImportManager::import_all_textures() {
        auto textures = registry_.get_assets_by_type(AssetType::TEXTURE);

        for (const auto &meta: textures) {
            // For textures, we just create metadata files if they don't exist
            auto meta_path = registry_.get_absolute_path(meta.uuid).string() + ".meta";

            if (!std::filesystem::exists(meta_path)) {
                std::cout << "Creating texture metadata: " << meta.name << std::endl;

                // Infer texture type from filename
                TextureMetadata tex_meta;
                tex_meta.type = infer_texture_type(meta.name);
                tex_meta.generate_mipmaps = true;
                tex_meta.srgb = (tex_meta.type == TextureType::DIFFUSE ||
                                 tex_meta.type == TextureType::EMISSIVE);

                TextureSerializer::save_metadata(
                    registry_.get_absolute_path(meta.uuid),
                    tex_meta
                );
            }
        }
    }

    bool AssetImportManager::import_asset(AssetID id) {
        auto meta = registry_.get_asset(id);
        if (!meta) return false;

        switch (meta->type) {
            case AssetType::MODEL:
                return import_model(*meta);
            case AssetType::TEXTURE:
                return import_texture(*meta);
            default:
                return false;
        }
    }

    bool AssetImportManager::needs_import(AssetID id) const {
        auto meta = registry_.get_asset(id);
        if (!meta) return false;

        // Check if .hfmodel file exists for this source
        auto imported_path = get_imported_path(*meta, ".hfmodel");
        if (!exists(imported_path)) {
            return true;
        }

        // Check if source is newer than imported
        auto source_path = registry_.get_absolute_path(id);
        auto source_time = last_write_time(source_path);
        auto imported_time = last_write_time(imported_path);

        return source_time > imported_time;
    }

    bool AssetImportManager::import_model(const AssetMetadata &meta) {
        auto source_path = project_root_ / meta.filepath;

        if (!std::filesystem::exists(source_path)) {
            std::cerr << "Source file not found: " << source_path << std::endl;
            return false;
        }

        // Create output directory for this model's assets
        auto model_output_dir = import_output_dir_ / meta.name;
        std::filesystem::create_directories(model_output_dir);

        // Import using ModelImporter
        ModelImporter importer(registry_, model_output_dir);
        ImportSettings settings;
        settings.generate_normals = true;
        settings.generate_tangents = true;
        settings.optimize_meshes = true;

        ImportResult result = importer.import(source_path, settings);

        if (!result.success) {
            std::cerr << "Failed to import model: " << meta.name
                    << " - " << result.error_message << std::endl;
            return false;
        }

        // Save the ImportResult as .hfmodel
        auto model_path = model_output_dir / (meta.name + ".hfmodel");
        if (!ModelSerializer::save(model_path, result)) {
            std::cerr << "Failed to save .hfmodel: " << model_path << std::endl;
            return false;
        }

        // Register the new .hfmodel in the registry
        registry_.register_asset(model_path, AssetType::MODEL);

        std::cout << "  Created: " << model_path.filename() << std::endl;
        std::cout << "  Meshes: " << result.created_mesh_assets.size() << std::endl;
        std::cout << "  Materials: " << result.created_material_assets.size() << std::endl;
        std::cout << "  Textures: " << result.created_texture_assets.size() << std::endl;

        return true;
    }

    bool AssetImportManager::import_texture(const AssetMetadata &meta) {
        // Textures don't need conversion, just metadata
        auto source_path = registry_.get_absolute_path(meta.uuid);

        TextureMetadata tex_meta;
        tex_meta.type = infer_texture_type(meta.name);
        tex_meta.generate_mipmaps = true;
        tex_meta.srgb = (tex_meta.type == TextureType::DIFFUSE);

        return TextureSerializer::save_metadata(source_path, tex_meta);
    }

    bool AssetImportManager::has_imported_mesh(AssetID original_id) const {
        return asset_manager_.get_mesh(original_id) != nullptr;
    }

    std::filesystem::path AssetImportManager::get_imported_path(const AssetMetadata &meta,
                                                                const std::string &extension) const {
        return import_output_dir_ / meta.name / (meta.name + extension);
    }


} // hellfire
