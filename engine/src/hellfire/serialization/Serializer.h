//
// Created by denzel on 03/12/2025.
//
#pragma once

#include <iosfwd>

#include "hellfire/ecs/TransformComponent.h"
#include "hellfire/scene/Scene.h"

// GLM Helper extensions
namespace glm {
    inline void to_json(nlohmann::json& j, const vec3& v) {
        j = {v.x, v.y, v.z};
    }
    
    inline void from_json(const nlohmann::json& j, vec3& v) {
        v = {j[0].get<float>(), j[1].get<float>(), j[2].get<float>()};
    }
}

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
            
            nlohmann::json j = {
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
                obj->set_position(j.at("position").get<glm::vec3>());
                obj->set_rotation(j.at("rotation").get<glm::vec3>());
                obj->set_scale(j.at("scale").get<glm::vec3>());
                return true;
            } catch (...) {
                return false;
            }
        }
    };
}
