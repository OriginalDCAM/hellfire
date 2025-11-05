//
// Created by denzel on 31/10/2025.
//

#include "AssetRegistry.h"

#include <fstream>
#include <iostream>
#include <ranges>
#include <thread>

#include "json.hpp"

namespace hellfire {
    AssetRegistry::AssetRegistry(const std::filesystem::path &registry_file, const std::filesystem::path &project_root) : registry_file_(registry_file), project_root_(project_root) {
        load();
    }

    AssetRegistry::~AssetRegistry() {
        save();
    }

    uint64_t AssetRegistry::register_asset(const std::filesystem::path &filepath) {
        AssetType type = get_type_from_extension(filepath);
        return register_asset(filepath, type);
        
    }

    uint64_t AssetRegistry::register_asset(const std::filesystem::path &filepath, AssetType type) {
        auto absolute_path = std::filesystem::absolute(filepath);

        // Check if already registered
        auto relative_path = to_relative_path(absolute_path);
        auto it = path_to_uuid_.find(relative_path);
        if (it != path_to_uuid_.end()) {
            // Update last modified time
            if (auto* asset = &assets_.at(it->second)) {
                asset->last_modified = get_file_last_modified(absolute_path);
            }
            return it->second;
        }

        // Generate UUID from relative path
        const AssetID uuid = generate_uuid(relative_path);

        const AssetMetadata metadata{
            .uuid = uuid,
            .filepath = relative_path,
            .type = type,
            .name = filepath.stem().string(),
            .last_modified = get_file_last_modified(absolute_path)
        };

        assets_[uuid] = metadata;
        path_to_uuid_[relative_path] = uuid;

        return uuid;
    }

    void AssetRegistry::unregister_asset(const AssetID uuid) {
        if (const auto it = assets_.find(uuid); it != assets_.end()) {
            path_to_uuid_.erase(it->second.filepath);
            assets_.erase(it);
        }
    }

    std::vector<AssetID> AssetRegistry::register_directory(const std::filesystem::path &directory_path, bool recursive) {
        std::vector<AssetID> registered_assets;
        auto absolute_dir = to_absolute_path(directory_path);

        if (!std::filesystem::exists(absolute_dir)) {
            return registered_assets;
        }

        auto iterator = std::filesystem::recursive_directory_iterator(absolute_dir);
        for (const auto& entry : iterator) {
            if (entry.is_regular_file()) {
                AssetType type = get_type_from_extension(entry.path());
                if (type != AssetType::UNKNOWN) {
                    AssetID id = register_asset(entry.path(), type);
                    registered_assets.push_back(id);
                }
            }
        }
        
        return registered_assets;
    }

    void AssetRegistry::refresh_assets() {
        for (auto &metadata: assets_ | std::views::values) {
            auto absolute_path = to_absolute_path(metadata.filepath);
            if (std::filesystem::exists(absolute_path)) {
                metadata.last_modified = get_file_last_modified(absolute_path);
            }
        }
    }

    std::optional<AssetMetadata> AssetRegistry::get_asset(uint32_t uuid) const {
        const auto it = assets_.find(uuid);
        if (it != assets_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::optional<uint64_t> AssetRegistry::get_uuid_by_path(const std::filesystem::path &filepath) {
        const auto relative_path = to_relative_path(filepath);
        const auto it = path_to_uuid_.find(relative_path);
        if (it != path_to_uuid_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::vector<AssetMetadata> AssetRegistry::get_assets_by_type(AssetType type) {
        std::vector<AssetMetadata> result;
        for (const auto &metadata: assets_ | std::views::values) {
            if (metadata.type == type) {
                result.push_back(metadata);
            }
        }
        return result;
    }

    std::vector<AssetMetadata> AssetRegistry::get_all_assets() const {
        std::vector<AssetMetadata> result;
        result.reserve(assets_.size());
        for (const auto &metadata: assets_ | std::views::values) {
            result.push_back(metadata);
        }
        return result;
    }

    bool AssetRegistry::asset_exists(AssetID uuid) const {
        return assets_.contains(uuid);
    }

    std::filesystem::path AssetRegistry::get_absolute_path(AssetID uuid) {
        if (const auto it = assets_.find(uuid); it != assets_.end()) {
            return to_absolute_path(it->second.filepath);
        }
        return {};
    }

    std::filesystem::path AssetRegistry::get_relative_path(AssetID uuid) {
        if (const auto it = assets_.find(uuid); it != assets_.end()) {
            return to_relative_path(it->second.filepath);
        }
        return {};
    }

    bool AssetRegistry::has_asset_changed(const AssetID uuid) const {
        auto it = assets_.find(uuid);
        if (it != assets_.end()) {
            auto absolute_path = to_absolute_path(it->second.filepath);
            if (std::filesystem::exists(absolute_path)) {
                const auto current_modified = get_file_last_modified(absolute_path);
                return current_modified != it->second.last_modified;
            }
        }
        return false;
    }

    std::vector<AssetID> AssetRegistry::get_modified_assets() const {
        std::vector<AssetID> modified;
        for (const auto &uuid: assets_ | std::views::keys) {
            if (has_asset_changed(uuid)) {
                modified.push_back(uuid);
            }
        }
        return modified;
    }

    void AssetRegistry::save() const {
        nlohmann::json j;
        j["version"] = "1.0";
        j["project_root"] = project_root_.string();
        j["assets"] = nlohmann::json::array();

        for (const auto &metadata: assets_ | std::views::values) {
            nlohmann::json asset_json;
            asset_json["uuid"] = metadata.uuid;
            asset_json["path"] = metadata.filepath.string();
            asset_json["type"] = metadata.type;
            asset_json["name"] = metadata.name;
            asset_json["last_modified"] = metadata.last_modified;
            j["assets"].push_back(asset_json);
        }

        std::ofstream file(registry_file_);
        file << j.dump(4);
    }

    bool AssetRegistry::load() {
        if (!std::filesystem::exists(registry_file_)) {
            return false;
        }

        try {
            std::ifstream file(registry_file_);
            nlohmann::json j;
            file >> j;

            if (j.contains("assets")) {
                for (const auto& asset_json : j["assets"]) {
                    AssetMetadata metadata{
                        .uuid = asset_json["uuid"].get<AssetID>(),
                        .filepath = asset_json["path"].get<std::string>(),
                        .type = asset_json["type"].get<AssetType>(),
                        .name = asset_json["name"].get<std::string>(),
                        .last_modified = asset_json["last_modified"].get<uint64_t>(),
                    };

                    assets_[metadata.uuid] = metadata;
                    path_to_uuid_[metadata.filepath] = metadata.uuid;
                }
            }
            return true;
        } catch (const std::exception& e) {
            std::cerr << "ERROR::ASSETREGISTRY::LOAD:: " << e.what() << std::endl;
            return false;
        }
    }

    void AssetRegistry::clear() {
        assets_.clear();
        path_to_uuid_.clear();
    }

    void AssetRegistry::set_project_root(const std::filesystem::path &project_root) {
        project_root_ = project_root;
    }

    AssetType AssetRegistry::get_type_from_extension(const std::filesystem::path &filepath) {
        static const std::unordered_map<std::string, AssetType> extension_map = {
            {".png", AssetType::TEXTURE},
            {".jpg", AssetType::TEXTURE},
            {".jpeg", AssetType::TEXTURE},
            {".obj", AssetType::MODEL},
            {".gltf", AssetType::MODEL},
            {".glb", AssetType::MODEL},
            {".fbx", AssetType::MODEL},
            {".hfmat", AssetType::MATERIAL},
            {".hfscene", AssetType::SCENE},
            {".frag", AssetType::SHADER},
            {".vert", AssetType::SHADER},
        };

        auto extension = filepath.extension().string();
        std::ranges::transform(extension, extension.begin(), tolower); // To make sure it's case-insensitive
        const auto it = extension_map.find(extension);

        return  (it != extension_map.end()) ? it->second : AssetType::UNKNOWN;
    }

    uint64_t AssetRegistry::generate_uuid(const std::filesystem::path& filepath) {
        std::hash<std::string> hasher;
        uint64_t id = hasher(filepath.string());
        return (id == INVALID_ASSET_ID) ? 1 : id;
    }

    std::filesystem::path AssetRegistry::to_relative_path(const std::filesystem::path &absolute_path) const {
        return std::filesystem::relative(absolute_path, project_root_);
    }

    std::filesystem::path AssetRegistry::to_absolute_path(const std::filesystem::path &relative_path) const {
        return project_root_ / relative_path;
    }

    void AssetRegistry::rebuild_path_map() {
        path_to_uuid_.clear();
        for (const auto& [uuid, metadata] : assets_) {
            path_to_uuid_[metadata.filepath] = uuid;
        }
    }

    uint64_t AssetRegistry::get_file_last_modified(const std::filesystem::path &filepath) const {
        if (std::filesystem::exists(filepath)) {
            const auto ftime = std::filesystem::last_write_time(filepath);
            return ftime.time_since_epoch().count();
        }
        return 0;
    }
}
