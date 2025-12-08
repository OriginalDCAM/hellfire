//
// Created by denzel on 05/12/2025.
//

#pragma once
#include <filesystem>
#include <string>

#include "hellfire/core/Project.h"
#include "UI/EventBus.h"

namespace hellfire::editor {
    class EditorContext;

    struct RecentProject {
        std::string name;
        std::filesystem::path path;
        std::string last_opened;
        bool exists = true;
    };

    struct ProjectTemplate {
        std::string id;
        std::string name;
        std::string description;
        std::filesystem::path source_path;
    };

    struct PathHash {
        std::size_t operator()(const std::filesystem::path& p) const {
            return std::filesystem::hash_value(p);
        }
    };

    // TODO: Add doxy gen comments
    class ProjectManager {
    public:
        explicit ProjectManager(EventBus &event_bus, EditorContext &context);

        // Current project
        Project *get_current_project() { return current_project_.get(); }
        bool has_project() { return current_project_ != nullptr; }

        // Project create/open/close
        void create_project_async(const std::string &name,
                                  const std::filesystem::path &location,
                                  const std::string &template_id);

        void open_project_async(const std::filesystem::path &project_file);

        void close_project();

        // Recent projects
        std::vector<RecentProject> get_recent_projects() const;
        void clear_recent_projects();
        void remove_from_recent(const std::filesystem::path& path);
        std::filesystem::path get_recent_projects_path() const {
            return config_path_ / "recent_projects.json";
        }

        // Templates
        std::vector<ProjectTemplate> get_templates() const;

    private:
        void add_to_recent(const std::string& name, const std::filesystem::path& path);
        void load_recent_projects();
        void save_recent_projects() const;

        void emit_progress(const std::string& message, float progress);
        
        EventBus &event_bus_;
        EditorContext &context_;
        std::unique_ptr<Project> current_project_;
        std::unordered_map<std::filesystem::path, RecentProject, PathHash> recent_projects_;
        std::filesystem::path config_path_;
    };
};
