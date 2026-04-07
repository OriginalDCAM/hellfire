//
// Created by denzel on 08/12/2025.
//

#pragma once
#include <cstdint>
#include <filesystem>

#include "hellfire/graphics/Mesh.h"

namespace hellfire {
    // Binary format for .hfmesh files
    class MeshSerializer {
    public:
        static constexpr uint32_t MAGIC = 0x4853454D; // MESH
        static constexpr uint32_t VERSION = 1;

        static bool save(const std::filesystem::path& filepath, const Mesh& mesh);
        static std::shared_ptr<Mesh> load(const std::filesystem::path& filepath);

        // JSON format for debugging/tools
        static bool save_json(const std::filesystem::path& filepath, const Mesh& mesh);
        static std::shared_ptr<Mesh> load_json(const std::filesystem::path& filepath);
    };
}
