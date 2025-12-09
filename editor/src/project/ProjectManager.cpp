//
// Created by denzel on 05/12/2025.
//

#include "ProjectManager.h"

#include <fstream>
#include <thread>

#include "events/StateEvents.h"
#include "hellfire/core/Time.h"
#include "serializers/ProjectManagerSerializer.h"
#include "ui/Panels/EditorPanel.h"

namespace hellfire::editor {
    ProjectManager::ProjectManager(EventBus &event_bus, EditorContext &context) : event_bus_(event_bus), context_(context) {

        // Platform-specific config path
#ifdef _WIN32
        config_path_ = std::filesystem::path(getenv("APPDATA")) / "Hellfire";
#else
        config_path_ = std::filesystem::path(getenv("HOME")) / ".config" / "hellfire";
#endif

        std::filesystem::create_directories(config_path_);
        load_recent_projects();
    }

    void ProjectManager::create_project_async(const std::string &name, const std::filesystem::path &location,
                                              const std::string &template_id) {
        std::thread([this, name, location, template_id]() {
            emit_progress("[INFO] Creating project folder: " + location.string(), 0.1f);
            auto project = Project::create(name, location);

            if (!project) {
                emit_progress("[ERROR] Failed to create project", 1.0f);
                return;
            }

            
            emit_progress("[INFO] Generated " + name + ".hfproj", 0.3f);
            emit_progress("[INFO] Created Assets/ folders", 0.5f);
            if (template_id != "empty") {
                emit_progress("[INFO] Applying template: " + template_id, 0.6f);
                // Copy template scenes, scripts, etc.
            }
            emit_progress("[INFO] Created Scenes/SampleScene.hfscene", 0.8f);
            emit_progress("[INFO] Initializing editor...", 0.9f);
            
            // Switch to main thread for final setup
            context_.queue_main_thread([this, p = std::move(project)]() mutable {
                current_project_ = std::move(p);
                current_project_->initialize_managers();
                add_to_recent(current_project_->get_name(), current_project_->get_project_root() / "project.hfproj");
                emit_progress("[INFO] Done!", 1.0f);
                event_bus_.dispatch<ProjectLoadCompleteEvent>();
            });


        }).detach();
    }

    void ProjectManager::open_project_async(const std::filesystem::path &project_file) {
        std::thread([this, project_file]() {
            emit_progress("[INFO] Loading project: " + project_file.string(), 0.2f);

            auto project = Project::load_data(project_file);
            if (!project) {
                emit_progress("[ERROR] Failed to load project", 1.0f);
                return;
            }
            
            emit_progress("[INFO] Done!", 1.0f);

            context_.queue_main_thread([this, p = std::move(project), project_file]() mutable {
                current_project_ = std::move(p);
                current_project_->initialize_managers();
                current_project_->get_metadata().last_opened = Time::get_current_timestamp();
                add_to_recent(current_project_->get_name(), project_file);
                event_bus_.dispatch<ProjectLoadCompleteEvent>();
            });
        }).detach();
    }

    void ProjectManager::close_project() {
        if (current_project_) {
            current_project_->save();
            current_project_.reset();
        }
    }

    std::vector<RecentProject> ProjectManager::get_recent_projects() const {
        std::vector<RecentProject> result;
        result.reserve(recent_projects_.size());
        
        for (const auto& [path, project] : recent_projects_) {
            result.push_back(project);
        }
        
        // Sort by last_opened (most recent first)
        std::sort(result.begin(), result.end(), [](const RecentProject& a, const RecentProject& b) {
            return a.last_opened > b.last_opened;
        });
        
        return result;
    }

    void ProjectManager::clear_recent_projects() {
            recent_projects_.clear();
    }

    void ProjectManager::remove_from_recent(const std::filesystem::path &path) {
        recent_projects_.erase(path);
        save_recent_projects();
    }

    std::vector<ProjectTemplate> ProjectManager::get_templates() const {
        return {
            {"empty", "Empty", "A blank project with no assets"},
            {"3d_prototype", "3D Prototype Starter", "Basic 3D scene with camera and lighting"},
            {"3d_fps", "3D FPS Starter", "First person controller with basic movement"}
        };
    }

    void ProjectManager::add_to_recent(const std::string &name, const std::filesystem::path &path) {
        recent_projects_[path] = RecentProject{
            .name = name,
            .path = path,
            .last_opened = current_project_->get_metadata().last_opened,
            .exists = true
        };
    
        // Keep max 10 - remove oldest if needed
        if (recent_projects_.size() > 10) {
            auto oldest = recent_projects_.begin();
            for (auto it = recent_projects_.begin(); it != recent_projects_.end(); ++it) {
                if (it->second.last_opened < oldest->second.last_opened) {
                    oldest = it;
                }
            }
            recent_projects_.erase(oldest);
        }
    
        save_recent_projects();
    }

    void ProjectManager::load_recent_projects() {
        std::ifstream file(get_recent_projects_path());
        if (!file.is_open()) return;

        Serializer<std::unordered_map<std::filesystem::path, RecentProject, PathHash>>::deserialize(file, &recent_projects_);
    }

    void ProjectManager::save_recent_projects() const {
        std::ofstream file(get_recent_projects_path());
        if (!file.is_open()) return;

        Serializer<std::unordered_map<std::filesystem::path, RecentProject, PathHash>>::serialize(file, &recent_projects_);
    }

    void ProjectManager::emit_progress(const std::string &message, float progress) {
        event_bus_.dispatch<ProjectLoadProgressEvent>(message, progress);
    }
}
