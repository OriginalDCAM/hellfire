//
// Created by denzel on 03/12/2025.
//
#pragma once

#include <iosfwd>

#include "hellfire/ecs/TransformComponent.h"
#include "hellfire/scene/Scene.h"
#include "hellfire/utilities/SerializerUtils.h"

namespace hellfire {
    template<typename T>
    struct Serializer {
        static bool serialize(std::ostream& output, const T* obj);
        static bool deserialize(std::istream& input, T* obj);
    };

    template<>
    struct Serializer<TransformComponent> {
        static bool serialize(std::ostream& output, const TransformComponent* obj) {
            if (obj == nullptr) return false;

            const nlohmann::json j = {
                {"position", obj->get_position()},
                {"rotation", obj->get_rotation()},
                {"scale", obj->get_scale()}
            };

            output << j.dump(4);
            return output.good();
        }

        static bool deserialize(std::istream& input, TransformComponent* obj) {
            try {
                if (obj == nullptr) return false;
                
                nlohmann::json j;
                input >> j;

                const auto position = json_get_vec3(j, "position");
                const auto rotation = json_get_vec3(j, "rotation");
                const auto scale = json_get_vec3(j, "scale");

                if (!position || !rotation || !scale) return false;
                
                obj->set_position(*position);
                obj->set_rotation(*rotation);
                obj->set_scale(*scale);
                
                return true;
            } catch (...) {
                return false;
            }
        }
    };
}
