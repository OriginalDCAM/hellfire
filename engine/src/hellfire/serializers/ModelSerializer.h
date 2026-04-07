//
// Created by denzel on 08/12/2025.
//

#pragma once
#include <cstdint>
#include <filesystem>
#include <optional>

#include "hellfire/assets/models/ImportResult.h"

namespace hellfire {
    /**
     * @brief Serializes the ImportResult to a .hfmodel file
     * 
     * This captures the hierarchy and asset references for instantiation
     */
    class ModelSerializer {
    public:
        static constexpr uint32_t MAGIC = 0x4C444F4D;  // "MODL"
        static constexpr uint32_t VERSION = 1;

        static bool save(const std::filesystem::path& filepath, const ImportResult& result);
        static std::optional<ImportResult> load(const std::filesystem::path& filepath);

        // JSON for debugging
        static bool save_json(const std::filesystem::path& filepath, const ImportResult& result);
        static std::optional<ImportResult> load_json(const std::filesystem::path& filepath);
    };
} // hellfire