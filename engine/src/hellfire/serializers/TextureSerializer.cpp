//
// Created by denzel on 08/12/2025.
//

#include "TextureSerializer.h"

#include <fstream>

#include "json.hpp"

namespace hellfire {
    static std::filesystem::path get_meta_path(const std::filesystem::path& texture_path) {
        return texture_path.string() + ".meta";
    }
    
    bool TextureSerializer::save_metadata(const std::filesystem::path &texture_path, const TextureMetadata &meta) {
        nlohmann::json j;

        j["type"] = static_cast<int>(meta.type);
        j["generate_mipmaps"] = meta.generate_mipmaps;
        j["srgb"] = meta.srgb;
        j["filter"] = static_cast<int>(meta.filter);
        j["wrap_u"] = static_cast<int>(meta.wrap_u);
        j["wrap_v"] = static_cast<int>(meta.wrap_v);
        j["compressed"] = meta.compressed;
        j["compression_format"] = meta.compression_format;

        std::ofstream file(get_meta_path(texture_path));
        if (!file) return false;

        file << j.dump(2);
        return file.good();
    }

    std::optional<TextureMetadata> TextureSerializer::load_metadata(const std::filesystem::path &texture_path) {
        const auto meta_path = get_meta_path(texture_path);
        if (!std::filesystem::exists(meta_path)) {
            return std::nullopt;  // No metadata, use defaults
        }

        std::ifstream file(meta_path);
        if (!file) return std::nullopt;

        try {
            nlohmann::json j;
            file >> j;

            TextureMetadata meta;
            meta.type = static_cast<TextureType>(j.value("type", 0));
            meta.generate_mipmaps = j.value("generate_mipmaps", true);
            meta.srgb = j.value("srgb", true);
            meta.filter = static_cast<TextureMetadata::FilterMode>(j.value("filter", 2));
            meta.wrap_u = static_cast<TextureMetadata::WrapMode>(j.value("wrap_u", 0));
            meta.wrap_v = static_cast<TextureMetadata::WrapMode>(j.value("wrap_v", 0));
            meta.compressed = j.value("compressed", false);
            meta.compression_format = j.value("compression_format", "");

            return meta;
        } catch (...) {
            return std::nullopt;
        }
    }
} // hellfire