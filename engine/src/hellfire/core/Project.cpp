//
// Created by denzel on 29/10/2025.
//

#include "Project.h"

namespace hellfire {
    Project::Project(const std::string &name) {
    }

    std::unique_ptr<Project> Project::create(const std::string &name, const std::filesystem::path &location) {
        return nullptr;
    }

    std::unique_ptr<Project> Project::load(const std::filesystem::path &project_file) {
        return nullptr;
    }

    bool Project::save() {
        return false;
    }

    void Project::close() {
    }

    std::filesystem::path Project::get_project_root() const {
        return {};
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
