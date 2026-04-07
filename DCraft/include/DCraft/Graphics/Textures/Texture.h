#pragma once
#include <cstdint>
#include <string>

namespace DCraft
{
    enum class TextureType {
        DIFFUSE,
        SPECULAR,
        NORMAL,
        AMBIENT_OCCLUSION,
        ROUGHNESS,
        METALLNESS
    };
    class Texture
    {
    public:
        int width, height;
        /// 
        /// Number of color channels
        /// 
        int nr_channels;

        [[nodiscard]]
        Texture(const std::string& path, TextureType type);
        void bind(unsigned int slot = 0);
        void unbind();

        TextureType get_type() const { return type_; }
        uint32_t get_id() { return texture_id_; }
        std::string get_path() { return path_; }

    private:
        TextureType type_;
        std::string path_;
        uint32_t texture_id_;
    };
}