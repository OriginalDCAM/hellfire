#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

#include "GL/glew.h"

namespace hellfire {
    using TextureHandle = uint32_t;
    
    enum class TextureType {
        DIFFUSE,
        SPECULAR,
        NORMAL,
        AMBIENT_OCCLUSION,
        ROUGHNESS,
        METALNESS,
        EMISSIVE,
        HEIGHT,
        OPACITY
    };
    
    enum class TextureFilter {
        NEAREST,
        LINEAR,
        LINEAR_MIPMAP_LINEAR,
        NEAREST_MIPMAP_NEAREST
    };

    enum class TextureWrap {
        REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER,
        MIRRORED_REPEAT
    };

    struct TextureSettings {
        TextureFilter min_filter = TextureFilter::LINEAR;
        TextureFilter mag_filter = TextureFilter::LINEAR;
        TextureWrap wrap_s = TextureWrap::REPEAT;
        TextureWrap wrap_t = TextureWrap::REPEAT;
        bool generate_mipmaps = true;
        bool flip_vertically = false;
        int desired_channels = 3;

        int max_size = -1;
    };

    class Texture {
    public:
      Texture(TextureHandle handle, TextureType type, int width, int height, int channels);
      
        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;
        Texture(Texture &&other) noexcept;
        Texture &operator=(Texture &&other) noexcept;
        ~Texture();

        void bind(unsigned int slot = 0) const;
        void unbind() const;

        TextureType get_type() const { return type_; }
        TextureHandle get_handle() const { return texture_id_; }
        [[nodiscard]] bool is_valid() const;

    private:
        TextureType type_;
        TextureHandle texture_id_ = 0;
        int width_ = 0;
        int height_ = 0;
        int nr_channels_ = 0;
        bool is_valid_ = false;
    };
    

}
