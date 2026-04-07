//
// Created by denzel on 29/12/2025.
//

#pragma once
#include "core/EditorApplication.h"
#include "hellfire/core/Application.h"
#include "hellfire/serializers/Serializer.h"


namespace hellfire {
    template<>
    struct Serializer<AppInfo> {
        static bool serialize(std::ostream &output, const AppInfo *app_info) {
            if (app_info == nullptr) return false;

            nlohmann::ordered_json j;

            j["width"] = app_info->width;
            j["height"] = app_info->height;

            output << j.dump(2);
            return output.good();
        }

        static bool deserialize(std::istream &input, AppInfo *app_info) {
            try {
                nlohmann::json j;
                input >> j;

                app_info->width = j.at("width").get<int>();
                app_info->height = j.at("height").get<int>();

                return true;
            } catch (...) {
                return false;
            }
        }
    };
}
