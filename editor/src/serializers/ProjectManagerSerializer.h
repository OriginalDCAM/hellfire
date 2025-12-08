//
// Created by denzel on 06/12/2025.
//

#pragma once
#include "hellfire/serializers/Serializer.h"
#include "project/ProjectManager.h"

namespace hellfire {
    template<>
    struct Serializer<std::unordered_map<std::filesystem::path, editor::RecentProject, editor::PathHash>> {
        using MapType = std::unordered_map<std::filesystem::path, editor::RecentProject, editor::PathHash>;
        
        static bool serialize(std::ostream& output, const MapType* recent_projects) {
            if (recent_projects == nullptr) return false;

            nlohmann::ordered_json j;
            
            for (const auto &project: *recent_projects | std::views::values) {
                nlohmann::ordered_json p;
                p["name"] = project.name;
                p["path"] = project.path.string();
                p["last_opened"] = project.last_opened;
                j.push_back(p);
            }

            output << j.dump(2);
            return output.good();
        }
        
        static bool deserialize(std::istream& input, MapType* recent_projects) {
            try {
                nlohmann::json j;
                input >> j;

                recent_projects->clear();
                
                for (const auto& project : j) {
                    editor::RecentProject recent_project;
                    recent_project.name = project.at("name");
                    recent_project.path = std::filesystem::path(project.at("path").get<std::string>());
                    recent_project.last_opened = project.at("last_opened");
                    recent_project.exists = std::filesystem::exists(recent_project.path);
                    
                    (*recent_projects)[recent_project.path] = recent_project;
                }

                return true;
            } catch (...) {
                return false;
            }
        }
    };
}
