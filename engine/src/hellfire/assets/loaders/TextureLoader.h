//
// Created by denzel on 07/04/2026.
//

#pragma once
#include <filesystem>
#include <memory>

#include "stb/stb_image.h"
#include "TextureLoader.h"
#include "hellfire/graphics/texture/Texture.h"

namespace hellfire {
    struct ImageData {
        std::unique_ptr<unsigned char, decltype(&stbi_image_free)> pixels{nullptr, stbi_image_free};
        int width = 0;
        int height = 0;
        int channels = 0;

        ImageData(unsigned char* data, const int w, const int h, const int c) : pixels(data, stbi_image_free), width(w), height(h), channels(c) {}
    };
    
    class TextureLoader {
    public:
        static std::optional<ImageData> load_from_disk(const std::filesystem::path& path, int desired_channels = 0);
        
        static std::unique_ptr<Texture> upload_to_gpu(const ImageData& image, TextureType type, const TextureSettings& settings = {});
        
        static std::unique_ptr<Texture> load(const std::filesystem::path &path, TextureType type, const TextureSettings& settings = {});
    };
} // hellfire