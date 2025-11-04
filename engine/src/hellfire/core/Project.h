//
// Created by denzel on 29/10/2025.
//

#pragma once
#include <filesystem>
#include <memory>

#include "hellfire/assets/AssetRegistry.h"
#include "hellfire/scene/SceneManager.h"

namespace hellfire {
    struct ProjectFileFormat {
        std::string name_;
        std::string version_;
        std::string engine_version_;
        std::string created_at_;
        std::string last_scene_opened_;
    };

    class Project {
    public:
        explicit Project(const std::string &name);

        static std::unique_ptr<Project> create(const std::string &name, const std::filesystem::path &location);

        static std::unique_ptr<Project> load(const std::filesystem::path &project_file);

        bool save();

        void close();

        // Project directory structure
        std::filesystem::path get_project_root() const;

        std::filesystem::path get_assets_path() const;

        std::filesystem::path get_scenes_path() const;

        std::filesystem::path get_scripts_path() const;

        // Getters/setters
        const std::string &get_name() const { return name_; }
        const std::string &get_version() const { return version_; }
        SceneManager *get_scene_manager() const { return scene_manager_.get(); }
        AssetRegistry *get_asset_registry() const { return asset_registry_.get(); }

    private:
        std::string name_;
        std::string version_;
        std::string engine_version_;
        std::string created_at_;
        std::filesystem::path last_scene_opened_;
        std::filesystem::path project_root_path_;
        std::filesystem::path project_file_path_;

        std::unique_ptr<hellfire::SceneManager> scene_manager_ = nullptr;
        std::unique_ptr<hellfire::AssetRegistry> asset_registry_ = nullptr;
        std::vector<std::filesystem::path> recent_scenes_;

    private:
        bool serialize() const;

        bool deserialize();

        void create_directory_structure();

        void initialize_default_assets();
    };
}
