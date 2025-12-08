//
// Created by denzel on 08/12/2025.
//

#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

#include "hellfire/assets/AssetRegistry.h"
#include "hellfire/graphics/texture/Texture.h"

namespace hellfire {
    /**
     * @brief Serializable material data (separate from runtime Material class)
     */
    struct MaterialData {
        std::string name = "Material";

        // Colors
        glm::vec3 diffuse_color{1.0f};
        glm::vec3 ambient_color{0.1f};
        glm::vec3 specular_color{1.0f};
        glm::vec3 emissive_color{0.0f};

        // Scalars
        float opacity = 1.0f;
        float shininess = 32.0f;
        float metallic = 0.0f;
        float roughness = 0.5f;

        // Texture references (by AssetID)
        std::unordered_map<TextureType, AssetID> texture_assets;

        // UV transforms (optional, per texture)
        glm::vec2 uv_scale{1.0f};
        glm::vec2 uv_offset{0.0f};

        // Rendering flags
        bool double_sided = false;
        bool alpha_blend = false;
        float alpha_cutoff = 0.5f;
    };
}
