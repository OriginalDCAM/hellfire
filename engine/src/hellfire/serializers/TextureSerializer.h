//
// Created by denzel on 08/12/2025.
//

#pragma once
#include <filesystem>

#include "hellfire/graphics/texture/Texture.h"

namespace hellfire {
    /**
     * @brief Metadata for texture assets
     *
     * The actual pixel data is stored in standard formats (PNG, JPG)
     */
    struct TextureMetadata {
        TextureType type = TextureType::DIFFUSE;
        bool generate_mipmaps = true;
        bool srgb = true;

        // Filtering
        enum class FilterMode { NEAREST, LINEAR, TRILINEAR };
        FilterMode filter = FilterMode::LINEAR;

        // Wrapping
        enum class WrapMode { REPEAT, CLAMP, MIRROR };
        WrapMode wrap_u = WrapMode::REPEAT;
        WrapMode wrap_v = WrapMode::REPEAT;

        // Compression (for baked assets)
        bool compressed = false;
        std::string compression_format;  // "BC1", "BC3", "BC7", etc.
    };
    
    class TextureSerializer {
    public:
        static bool save_metadata(const std::filesystem::path& texture_path, 
                                  const TextureMetadata& meta);
        
        static std::optional<TextureMetadata> load_metadata(
            const std::filesystem::path& texture_path);
    };
} // hellfire