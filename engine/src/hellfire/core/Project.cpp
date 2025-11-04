//
// Created by denzel on 29/10/2025.
//

#include "Project.h"

#include <fstream>
#include <iostream>

#include "json.hpp"

namespace hellfire {
    Project::Project(const std::string &name) {
    }

    std::unique_ptr<Project> Project::create(const std::string &name, const std::filesystem::path &location) {
        auto project = std::make_unique<Project>(name);
        project->project_root_path_ = location / name;
        project->project_file_path_ = project->project_root_path_ / "project.hfproj";

        // Create directory structure
        project->create_directory_structure();

        // Initialize managers with project root context
        auto registry_path = project->project_root_path_ / "assetregistry.json";
        project->asset_registry_ = std::make_unique<AssetRegistry>(registry_path, project->project_root_path_);

        project->scene_manager_ = std::make_unique<SceneManager>();

        // Initialize default assets
        project->initialize_default_assets();

        // Save project file
        project->serialize();
        
        return project;
    }

    std::unique_ptr<Project> Project::load(const std::filesystem::path &project_file) {
        if (!std::filesystem::exists(project_file)) {
            std::cerr << "ERROR::PROJECT::LOAD:: Project file doesnt exist at " << project_file.string() << std::endl;
            return nullptr;
        }

        try {
            std::ifstream file(project_file);
            nlohmann::json j;
            file >> j;

            if (j.contains("name")) {
                auto project = std::make_unique<Project>(j["name"].get<std::string>());

                if (j.contains("version")) {
                    project->version_ = j["version"].get<std::string>();
                }

                if (j.contains("engine_version")) {
                    project->engine_version_ = j["engine_version"].get<std::string>();
                }
            }
        }
    }

    bool Project::save() {
        return false;
    }

    void Project::close() {
    }

    std::filesystem::path Project::get_project_root() const {
        return project_root_path_;
    }

    std::filesystem::path Project::get_assets_path() const {
        return {};
    }

    std::filesystem::path Project::get_scenes_path() const {
        return {};
    }

    std::filesystem::path Project::get_scripts_path() const {
        return {};
    }

    bool Project::serialize() const {
        return false; 
    }

    bool Project::deserialize() {
        return false;
    }

    void Project::create_directory_structure() {
    }

    void Project::initialize_default_assets() {
    }
}
