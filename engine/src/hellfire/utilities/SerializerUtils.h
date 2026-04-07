//
// Created by denzel on 08/12/2025.
//

#pragma once
#include <optional>

#include "glm/glm/detail/type_vec.hpp"
#include "nlohmann/json.hpp"

namespace hellfire {

    inline std::optional<glm::vec3> json_to_vec3(const nlohmann::json& j) {
        if (!j.is_array() || j.size() != 3) return std::nullopt;
        
        for (const auto& elem : j) {
            if (!elem.is_number()) return std::nullopt;
        }
        
        return glm::vec3(j[0].get<float>(), j[1].get<float>(), j[2].get<float>());
    }

    inline std::optional<glm::vec3> json_get_vec3(const nlohmann::json& j, const std::string& key) {
        if (!j.contains(key)) return std::nullopt;
        return json_to_vec3(j.at(key));
    }

} // namespace hellfire

// GLM Helper extensions
namespace glm {
    inline void to_json(nlohmann::json& j, const vec3& v) {
        j = {v.x, v.y, v.z};
    }
    
    inline void from_json(const nlohmann::json& j, vec3& v) {
        v = {j[0].get<float>(), j[1].get<float>(), j[2].get<float>()};
    }
} // namespace glm
