//
// Created by denzel on 29/10/2025.
//

#include "Project.h"

#include <fstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui_internal.h"
#include "json.hpp"
#include "hellfire/assets/AssetManager.h"
#include "hellfire/assets/importers/AssetImportManager.h"
#include "hellfire/scene/Scene.h"
#include "hellfire/serializers/ProjectSerializer.h"
#include "hellfire/utilities/ServiceLocator.h"

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

    Project::~Project() {
        cleanup_managers();
    }

    std::unique_ptr<Project> Project::create(const std::string &name, const std::filesystem::path &location) {
        ProjectMetadata metadata;
        metadata.name = name;
        metadata.version = "1.0.0";
        metadata.engine_version = "0.1.0";
    
        // Get timestamp
        metadata.created_at = get_current_timestamp();
        metadata.last_opened = metadata.created_at;
    
        auto project = std::make_unique<Project>(metadata);
        project->project_root_path_ = location / name;
        project->project_file_path_ = project->project_root_path_ / "project.hfproj";
    
        project->create_directory_structure();
    
        if (!project->save()) {
            std::cerr << "Failed to save new project" << std::endl;
            return nullptr;
        }
    
        return project;
    }

    std::unique_ptr<Project> Project::load_data(const std::filesystem::path &project_file) {
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
            
            ProjectMetadata metadata;
            if (!Serializer<ProjectMetadata>::deserialize(file, &metadata)) {
                std::cerr << "Failed to deserialize project metadata" << std::endl;
                return nullptr;
            }

            // Create project with metadata
            auto project = std::make_unique<Project>(metadata);
            project->project_file_path_ = project_file;
            project->project_root_path_ = project_file.parent_path();

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

        std::ofstream file(project_file_path_);
        if (!file.is_open()) return false;

        // Save asset registry
        if (asset_registry_) {
            asset_registry_->save();
        }

        // Save current scene if loaded
        if (scene_manager_ && scene_manager_->has_active_scene()) {
            scene_manager_->save_current_scene();
            metadata_.last_scene = scene_manager_->get_active_scene_asset_id();
        }


        return Serializer<ProjectMetadata>::serialize(file, &metadata_);
    }

    void Project::close() {
        save();
        
        if (scene_manager_) {
            scene_manager_->clear();
        }
        
        if (asset_registry_) {
            asset_registry_->clear();
        }

        cleanup_managers();
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

    void Project::create_directory_structure() const {
        create_directories(project_root_path_ / "settings");
        create_directories(project_root_path_ / "assets");
        create_directories(project_root_path_ / "assets" / "scenes");
        create_directories(project_root_path_ / "assets" / "textures");
        create_directories(project_root_path_ / "assets" / "models");
        create_directories(project_root_path_ / "assets" / "scripts");
    }

    void Project::initialize_default_assets() {
    }


    void Project::initialize_managers() {
        ServiceLocator::unregister_service<SceneManager>();
        ServiceLocator::unregister_service<AssetRegistry>();
        
        // Initialize managers with project root context
        auto registry_path = project_root_path_ / "settings/assetregistry.json";
        asset_registry_ = std::make_unique<AssetRegistry>(registry_path, project_root_path_);
        ServiceLocator::register_service<AssetRegistry>(asset_registry_.get());
        asset_registry_->register_directory(get_assets_path(), true);

        asset_manager_ = std::make_unique<AssetManager>(*asset_registry_.get());
        ServiceLocator::register_service<AssetManager>(asset_manager_.get());

        AssetImportManager import_manager(*asset_registry_, *asset_manager_, project_root_path_);
        import_manager.import_all_pending();
        asset_registry_->save();

        scene_manager_ = std::make_unique<SceneManager>();
        ServiceLocator::register_service<SceneManager>(scene_manager_.get());

        // Load last scene if exists
        if (metadata_.last_scene) {
            if (auto asset_info = asset_registry_->get_asset(metadata_.last_scene.value())) {
                auto path = asset_registry_->get_absolute_path(asset_info->uuid);
                auto scene = scene_manager_->load_scene(asset_info->uuid, path);
                scene_manager_->set_active_scene(scene);
            }
        }
    }

    void Project::cleanup_managers() {
        ServiceLocator::unregister_service<SceneManager>();
        ServiceLocator::unregister_service<AssetRegistry>();
        ServiceLocator::unregister_service<AssetManager>();
        scene_manager_.reset();
        asset_registry_.reset();
    }

    std::string Project::get_current_timestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&time_t_now), "%Y-%m-%dT%H:%M:%SZ");
        return ss.str();
    }
}
