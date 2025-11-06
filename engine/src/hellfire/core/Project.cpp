//
// Created by denzel on 29/10/2025.
//

#include "Project.h"

#include <fstream>
#include <iostream>

#include "json.hpp"

namespace hellfire {
    Project::Project(const std::string &name) {
        metadata_.name = name;
        metadata_.version = "1.0.0";
        metadata_.engine_version = "0.1.0";
        metadata_.created_at = get_current_timestamp();
        metadata_.last_opened = metadata_.created_at;
        metadata_.default_scene = "default_scene.hfscene";
        metadata_.renderer_settings = "settings/renderer.json";
    }

    Project::Project(const ProjectMetadata &metadata) : metadata_(metadata) {}

    std::unique_ptr<Project> Project::create(const std::string &name, const std::filesystem::path &location) {
        auto project = std::make_unique<Project>(name);
        
        project->project_root_path_ = location / name;
        project->project_file_path_ = project->project_root_path_ / "project.hfproj";

        // Create directory structure
        project->create_directory_structure();

        project->initialize_managers();

        // Initialize default assets
        project->initialize_default_assets();

        // Save project file
        if (!project->serialize()) {
            std::cerr << "ERROR::PROJECT::CREATE:: Failed to save project file" << std::endl;
            return nullptr;
        }

        std::cout << "Succesfully create project:" << name << std::endl;
        return project;
    }

    std::unique_ptr<Project> Project::load(const std::filesystem::path &project_file) {
        if (!exists(project_file)) {
            std::cerr << "ERROR::PROJECT::LOAD:: Project file doesnt exist at " << project_file.string() << std::endl;
            return nullptr;
        }

        try {
            std::ifstream file(project_file);
            if (!file.is_open()) {
                std::cerr << "ERROR::PROJECT::LOAD:: Failed to open project file" << std::endl;
                return nullptr;
            }

            // Parse file content as json
            nlohmann::json j;
            file >> j;

            if (!j.contains("name")) {
                std::cerr << "ERROR::PROJECT::LOAD:: Missing required 'name' field" << std::endl;
                return nullptr;
            }

            // Deserialize metadata
            ProjectMetadata metadata;


            if (j.contains("version")) {
                metadata.version = j["version"].get<std::string>();
            }

            if (j.contains("engine_version")) {
                metadata.engine_version = j["engine_version"].get<std::string>();
            }
            
            if (j.contains("created")) {
                metadata.created_at = j["created"].get<std::string>();
            }
            
            if (j.contains("last_opened")) {
                metadata.last_opened= j["last_opened"].get<std::string>();
            }
            
            if (j.contains("last_scene")) {
                metadata.last_scene = j["last_scene"].get<std::string>();
            }
            
            if (j.contains("settings")) {
                const auto& settings = j["settings"];

                if (settings.contains("default_scene")) {
                    metadata.default_scene = settings["default_scene"].get<std::string>();
                }

                if (settings.contains("renderer_settings")) {
                    metadata.renderer_settings = settings["renderer_settings"].get<std::string>();
                }
            }

            // Create project with metadata
            auto project = std::make_unique<Project>(metadata);
            project->project_file_path_ = project_file;
            project->project_root_path_ = project_file.parent_path();

            // Initialize managers
            project->initialize_managers();

            // Load the last opened scene if it exists
            if (!metadata.last_scene.empty()) {
                auto scene_path = project->project_root_path_ / metadata.last_scene;
                if (std::filesystem::exists(scene_path)) {
                    project->scene_manager_->load_scene(scene_path);
                }
            }

            std::cout << "Succesfully loaded project:" << metadata.name << std::endl;
            return project;
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "ERROR::PROJECT::LOAD:: JSON parse error: " << e.what() << std::endl;
        }
        catch (const std::exception &e) {
            std::cout << "ERROR::PROJECT::LOAD:: Exception: " << e.what() << std::endl;
            return nullptr;
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
        return project_root_path_ / "assets";
    }

    std::filesystem::path Project::get_scenes_path() const {
        return project_root_path_ / "assets/scenes";
    }

    std::filesystem::path Project::get_settings_path() const {
        return project_root_path_ / "settings";
    }

    bool Project::serialize() const {
    }

    bool Project::deserialize() {
    }

    void Project::create_directory_structure() const {
        std::filesystem::create_directories(project_root_path_/ "settings");
        std::filesystem::create_directories(project_root_path_/ "assets");
        std::filesystem::create_directories(project_root_path_/ "shared");
        
    }

    void Project::initialize_default_assets() {
    }


    void Project::initialize_managers() {
        // Initialize managers with project root context
        auto registry_path = project_root_path_ / "settings/assetregistry.json";
        asset_registry_ = std::make_unique<AssetRegistry>(registry_path, project_root_path_);

        scene_manager_ = std::make_unique<SceneManager>();

    }

    std::string Project::get_current_timestamp() const {
    }
}
