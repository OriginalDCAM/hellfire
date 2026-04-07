//
// Created by denzel on 08/12/2025.
//

#pragma once
#include <string>
#include <unordered_map>

#include "glm/glm.hpp"
#include "hellfire/assets/AssetRegistry.h"
#include "hellfire/graphics/material/MaterialData.h"
#include "hellfire/graphics/texture/Texture.h"

namespace hellfire {

    class MaterialSerializer {
    public:
        static constexpr uint32_t MAGIC = 0x54414D48;  // "HMAT"
        static constexpr uint32_t VERSION = 1;

        // Binary format (.hfmat)
        static bool save(const std::filesystem::path& filepath, const MaterialData& material);
        static std::optional<MaterialData> load(const std::filesystem::path& filepath);

        // JSON format for debugging/tools
        static bool save_json(const std::filesystem::path& filepath, const MaterialData& material);
        static std::optional<MaterialData> load_json(const std::filesystem::path& filepath);
    };
}
