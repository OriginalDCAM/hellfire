//
// Created by denzel on 31/10/2025.
//

#pragma once
#include <filesystem>
#include <string>
#include <unordered_map>

namespace hellfire {
    using AssetID = uint64_t;
    constexpr AssetID INVALID_ASSET_ID = 0;
    
    enum class AssetType {
        MODEL,
        MESH,
        TEXTURE,
        MATERIAL,
        SCENE,
        SHADER,
        UNKNOWN, 
    };

    struct AssetMetadata {
        uint64_t uuid;
        std::filesystem::path filepath; // Relative to project root
        AssetType type;
        std::string name;
        uint64_t last_modified; // For detecting changes
    };

    /**
     * @brief Registry for storing assets
     */
    class AssetRegistry {
    public:
        explicit AssetRegistry(const std::filesystem::path &registry_file, const std::filesystem::path &project_root);

        ~AssetRegistry();

        // Registration
        AssetID register_asset(const std::filesystem::path &filepath);
        AssetID register_asset(const std::filesystem::path &filepath, AssetType type);
        void unregister_asset(AssetID uuid);

        std::vector<AssetID> register_directory(const std::filesystem::path &directory_path, bool recursive);
        void refresh_assets(); // Re-scan and update modified times

        // Lookup
        std::optional<AssetMetadata> get_asset(AssetID uuid) const;
        std::optional<AssetID> get_uuid_by_path(const std::filesystem::path &filepath);
        std::vector<AssetMetadata> get_assets_by_type(AssetType type);
        std::vector<AssetMetadata> get_all_assets() const;

        // Path utilities
        bool asset_exists(AssetID uuid) const;
        std::filesystem::path get_absolute_path(AssetID uuid);
        std::filesystem::path get_relative_path(AssetID uuid);

        bool has_asset_changed(AssetID uuid) const;
        std::vector<AssetID> get_modified_assets() const;

        // Serialization
        void save();
        bool load();

        // Utility methods
        void clear();
        size_t get_asset_count() const { return assets_.size(); }

        // Project root management
        void set_project_root(const std::filesystem::path &project_root);
        const std::filesystem::path& get_project_root() { return project_root_; }

        static AssetType get_type_from_extension(const std::filesystem::path &filepath);
    private:
        std::unordered_map<AssetID, AssetMetadata> assets_;
        std::unordered_map<std::filesystem::path, AssetID> path_to_uuid_;
        std::filesystem::path registry_file_;
        std::filesystem::path project_root_;

        // Generate UUID from filepath
        AssetID generate_uuid(const std::filesystem::path &filepath);

        // Convert between absolute and relative paths
        std::filesystem::path to_relative_path(const std::filesystem::path &absolute_path) const;
        std::filesystem::path to_absolute_path(const std::filesystem::path &relative_path) const;

        // Internal helpers
        void rebuild_path_map();
        uint64_t get_file_last_modified(const std::filesystem::path &filepath) const;
    };
}
