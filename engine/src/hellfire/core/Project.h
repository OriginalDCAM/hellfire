//
// Created by denzel on 29/10/2025.
//

#pragma once
#include <filesystem>
#include <memory>

#include "hellfire/assets/AssetManager.h"
#include "hellfire/assets/AssetRegistry.h"
#include "hellfire/scene/SceneManager.h"

namespace hellfire {
    struct ProjectMetadata {
        std::string name;
        std::string version;
        std::string engine_version;
        std::string created_at;
        std::string last_opened;
        std::optional<AssetID> last_scene;
        std::filesystem::path default_scene;
        std::filesystem::path renderer_settings;
    };

    class Project {
    public:
        explicit Project(const std::string &name);
        explicit Project(const ProjectMetadata &metadata);
        ~Project();

        static std::unique_ptr<Project> create(const std::string &name, const std::filesystem::path &location);
        static std::unique_ptr<Project> load_data(const std::filesystem::path &project_file);

        void initialize_managers();

        bool save();
        void close();

        // Project directory structure
        std::filesystem::path get_project_root() const;
        std::filesystem::path get_assets_path() const;
        std::filesystem::path get_scenes_path() const;
        std::filesystem::path get_settings_path() const;

        // Getters/setters
        const ProjectMetadata& get_metadata() const { return metadata_; }
        ProjectMetadata& get_metadata() { return metadata_; }
        
        const std::string &get_name() const { return metadata_.name; }
        const std::string &get_version() const { return metadata_.version; }
        
        SceneManager *get_scene_manager() const { return scene_manager_.get(); }
        AssetRegistry *get_asset_registry() const { return asset_registry_.get(); }
    private:
        ProjectMetadata metadata_;
        std::filesystem::path project_root_path_;
        std::filesystem::path project_file_path_;

        std::unique_ptr<SceneManager> scene_manager_ = nullptr;
        std::unique_ptr<AssetRegistry> asset_registry_ = nullptr;
        std::unique_ptr<AssetManager> asset_manager_ = nullptr;
        
        std::vector<std::filesystem::path> recent_scenes_;

    private:
        void create_directory_structure() const;
        void initialize_default_assets();
        void cleanup_managers();


        static std::string get_current_timestamp();
    };
}
