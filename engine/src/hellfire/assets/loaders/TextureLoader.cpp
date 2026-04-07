//
// Created by denzel on 07/04/2026.
//

#include "TextureLoader.h"

#include <fstream>
#include <iostream>

#include "stb_image.h"

namespace hellfire {
    std::unique_ptr<Texture> TextureLoader::load(const std::filesystem::path &path, TextureType type) {
        int tex_id = 0;

        
        // Generate texture handle
        
        // Bind & store texture handle
        
        // Determine the format?
        
        // Upload to the GPU and check for success should return boolean
        
        // Generate mipmaps and set parameters
        
        // Clean up and mark the texture as valid
        
        // return a unique_ptr texture if everything went good
    }

    std::optional<ImageData> TextureLoader::load_from_disk(const std::filesystem::path &path, int desired_channels) {
        int width = 0;
        int height = 0;
        int nr_channels = 0;
        
        // Check if file exists
        if (const std::ifstream file(path); !file.good()) {
            std::cerr << "Texture file does not exist at: " << path << std::endl;
            return std::nullopt;
        }
        
        // Set the stbi settings
        unsigned char *data = stbi_load(path.string().c_str(), &width, &height, &nr_channels, desired_channels);
        
        if (!data) {
            const char *error = stbi_failure_reason();
            std::cerr << "stbi failed to load: " << path
                    << " - " << (error ? error : "Unknown error") << std::endl;
            return std::nullopt;
        }
        
        // Validate the loaded image data
        if (width <= 0 || height <= 0 || nr_channels <= 0) {
            std::cerr << "Invalid texture data: " << width << "x" << height
                    << " channels=" << nr_channels << std::endl;
            stbi_image_free(data);
            return std::nullopt;
        }
        
        if (desired_channels > 0) {
            nr_channels = desired_channels;
        }
        
        return ImageData{ data, width, height, nr_channels};
    }

    std::unique_ptr<Texture> TextureLoader::upload_to_gpu(const ImageData &image, TextureType type,
        const TextureSettings &settings) {
    }

    std::unique_ptr<Texture> TextureLoader::load(const std::filesystem::path &path, TextureType type,
        const TextureSettings &settings) {
    }
} // hellfire