//
// Created by denzel on 29/10/2025.
//

#include "Project.h"

#include <fstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "json.hpp"

namespace hellfire {
    Project::Project(const std::string &name) {
        metadata_.name = name;
        metadata_.version = "1.0.0";
        metadata_.engine_version = "0.1.0";
        metadata_.created_at = get_current_timestamp();
        metadata_.last_opened = metadata_.created_at;
        metadata_.default_scene = "assets/default_scene.hfscene";
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

            std::cout << "Successfully loaded project: " << metadata.name << std::endl;
            return project;
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "ERROR::PROJECT::LOAD:: JSON parse error: " << e.what() << std::endl;
        }
        catch (const std::exception &e) {
            std::cout << "ERROR::PROJECT::LOAD:: Exception: " << e.what() << std::endl;
            return nullptr;
        }
        return nullptr;
    }

    bool Project::save() {
        // Update last_opened timestamp
        metadata_.last_opened = get_current_timestamp();
        
        // Serialize project file
        if (!serialize()) {
            std::cerr << "ERROR::PROJECT::SAVE:: Failed to serialize project" << std::endl;
            return false;
        }

        // Save asset registry
        if (asset_registry_) {
            asset_registry_->save();
        }

        // Save current scene if loaded
        if (scene_manager_ && scene_manager_->has_active_scene()) {
            scene_manager_->save_current_scene();
        }

        return true;
    }

    void Project::close() {
        save();
        
        if (scene_manager_) {
            scene_manager_->clear();
        }
        
        if (asset_registry_) {
            asset_registry_->clear();
        }
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
        try {
            nlohmann::json j;
            
            // Serialize metadata
            j["name"] = metadata_.name;
            j["version"] = metadata_.version;
            j["engine_version"] = metadata_.engine_version;
            j["created"] = metadata_.created_at;
            j["last_opened"] = metadata_.last_opened;
            
            if (!metadata_.last_scene.empty()) {
                j["last_scene"] = metadata_.last_scene.string();
            }

            // Serialize settings
            j["settings"] = {
                {"default_scene", metadata_.default_scene.string()},
                {"renderer_settings", metadata_.renderer_settings.string()}
            };

            // Write to file
            std::ofstream file(project_file_path_);
            if (!file.is_open()) {
                return false;
            }

            file << j.dump(4); // Pretty print
            return true;

        } catch (const std::exception& e) {
            std::cerr << "ERROR::PROJECT::SERIALIZE:: " << e.what() << std::endl;
            return false;
        }
    }

    void Project::create_directory_structure() const {
        create_directories(project_root_path_/ "settings");
        create_directories(project_root_path_/ "assets");
        create_directories(project_root_path_/ "shared");
        
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
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&time_t_now), "%Y-%m-%dT%H:%M:%SZ");
        return ss.str();
    }
}
