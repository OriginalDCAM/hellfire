//
// Created by denzel on 29/10/2025.
//

#include "Project.h"

namespace hellfire {
    Project::Project(const std::string &name) {
    }

    std::unique_ptr<Project> Project::create(const std::string &name, const std::filesystem::path &location) {
    }

    std::unique_ptr<Project> Project::load(const std::filesystem::path &project_file) {
    }

    bool Project::save() {
    }

    void Project::close() {
    }

    std::filesystem::path Project::get_project_root() const {
    }

    std::filesystem::path Project::get_assets_path() const {
    }

    std::filesystem::path Project::get_scenes_path() const {
    }

    std::filesystem::path Project::get_scripts_path() const {
    }

    bool Project::serialize() const {
    }

    bool Project::deserialize() {
    }

    void Project::create_directory_structure() {
    }

    void Project::initialize_default_assets() {
    }
}
