//
// Created by denzel on 06/12/2025.
//

#pragma once
#include "hellfire/serialization/Serializer.h"
#include "hellfire/core/Project.h"

namespace hellfire {
    template<>
    struct Serializer<ProjectMetadata> {
        static bool serialize(std::ostream &output, const ProjectMetadata *obj) {
            if (!obj) return false;

            nlohmann::json j = {
                {"name", obj->name},
                {"version", obj->version},
                {"engine_version", obj->engine_version},
                {"created", obj->created_at},
                {"last_opened", obj->last_opened},
                {
                    "settings", {
                        {"default_scene", obj->default_scene.string()},
                        {"renderer_settings", obj->renderer_settings.string()}
                    }
                }
            };

            if (obj->last_scene.has_value()) {
                j["last_scene"] = obj->last_scene.value();
            }

            output << j.dump(2);
            return output.good();
        }

        static bool deserialize(std::istream &input, ProjectMetadata *obj) {
            try {
                if (!obj) return false;

                nlohmann::json j;
                input >> j;
            
                obj->name = j.at("name").get<std::string>();
                obj->version = j.value("version", "1.0.0");
                obj->engine_version = j.value("engine_version", "");
                obj->created_at = j.value("created", "");
                obj->last_opened = j.value("last_opened", "");
            
                if (j.contains("last_scene")) {
                    obj->last_scene = j["last_scene"].get<AssetID>();
                }
            
                if (j.contains("settings")) {
                    const auto& settings = j["settings"];
                    obj->default_scene = settings.value("default_scene", "");
                    obj->renderer_settings = settings.value("renderer_settings", "");
                }
            
                return true;
            } catch (const std::exception& e) {
                std::cerr << "ProjectMetadata deserialize error: " << e.what() << std::endl;
                return false;
            }
        }
    };
}
