#include "hellfire/graphics/texture/Texture.h"

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <fstream>
#include <iostream>
#include <stb/stb_image.h>

#include "hellfire/graphics/material/Material.h"

namespace hellfire {

    Texture::Texture(Texture &&other) noexcept
        : type_(other.type_), texture_id_(other.texture_id_), width_(other.width_),
          height_(other.height_),
          nr_channels_(other.nr_channels_) {
        other.texture_id_ = 0; // Transfer ownership
    }

    Texture &Texture::operator=(Texture &&other) noexcept {
        if (this != &other) {
            // Clean up current texture
            if (texture_id_ != 0) {
                glDeleteTextures(1, &texture_id_);
            }

            // Transfer ownership
            width_ = other.width_;
            height_ = other.height_;
            nr_channels_ = other.nr_channels_;
            type_ = other.type_;
            texture_id_ = other.texture_id_;

            other.texture_id_ = 0;
        }
        return *this;
    }

    Texture::~Texture() {
        if (texture_id_ != 0) {
            glDeleteTextures(1, &texture_id_);
        }
    }


    bool Texture::is_valid() const {
        return texture_id_ != 0 && width_ > 0 && height_ > 0;
    }

    Texture::Texture(const TextureHandle handle, const TextureType type, int width, int height, int channels) : type_(type), texture_id_(handle), width_(width), height_(height), nr_channels_(channels)  {}

    void Texture::bind(unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
    }

    void Texture::unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // GLint Texture::get_gl_wrap_mode(const TextureWrap wrap) const {
    //     switch (wrap) {
    //         case TextureWrap::REPEAT: return GL_REPEAT;
    //         case TextureWrap::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
    //         case TextureWrap::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
    //         case TextureWrap::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
    //         default: return GL_REPEAT;
    //     }
    // }

    // GLint Texture::get_gl_filter_mode(TextureFilter filter) const {
    //     switch (filter) {
    //         case TextureFilter::NEAREST: return GL_NEAREST;
    //         case TextureFilter::LINEAR: return GL_LINEAR;
    //         case TextureFilter::LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
    //         case TextureFilter::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
    //         default: return GL_LINEAR;
    //     }
    // }
}
