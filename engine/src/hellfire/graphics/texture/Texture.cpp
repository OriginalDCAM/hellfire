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

    void Texture::load_texture_data() {
        texture_id_ = 0;
        width = height = nr_channels = 0;
        is_valid_ = false;

        // Check if file exists
        if (const std::ifstream file(path_); !file.good()) {
            std::cerr << "Texture file does not exist: " << path_ << std::endl;
            return;
        }

        // Set STBI settings
        stbi_set_flip_vertically_on_load(settings_.flip_vertically);
        int desired_channels = 0;
        if (type_ == TextureType::DIFFUSE) {
            desired_channels = 3;
        } else if (type_ == TextureType::ROUGHNESS || type_ == TextureType::METALNESS ||
                   type_ == TextureType::AMBIENT_OCCLUSION) {
            desired_channels = 1;
        }

        unsigned char *data = stbi_load(path_.c_str(), &width, &height, &nr_channels, desired_channels);

        if (desired_channels > 0) {
            nr_channels = desired_channels;
        }

        if (!data) {
            const char *error = stbi_failure_reason();
            std::cerr << "STBI failed to load: " << path_
                    << " - " << (error ? error : "Unknown error") << std::endl;
            return;
        }

        // Validate loaded parts
        if (width <= 0 || height <= 0 || nr_channels <= 0) {
            std::cerr << "Invalid texture data: " << width << "x" << height
                    << " channels=" << nr_channels << std::endl;
            stbi_image_free(data);
            return;
        }

        // Generate OpenGL texture
        glGenTextures(1, &texture_id_);
        if (texture_id_ == 0) {
            std::cerr << "Failed to generate OpenGL texture" << std::endl;
            stbi_image_free(data);
            return;
        }

        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Determine formats
        GLenum format, internal_format;
        switch (nr_channels) {
            case 1:
                format = GL_RED;
                internal_format = GL_R8;
                break;
            case 3:
                format = GL_RGB;
                internal_format = GL_RGB8;
                break;
            case 4:
                format = GL_RGBA;
                internal_format = GL_RGBA8;
                break;
            default:
                std::cerr << "Unsupported channel count: " << nr_channels << std::endl;
                stbi_image_free(data);
                glDeleteTextures(1, &texture_id_);
                texture_id_ = 0;
                return;
        }

        // Handle special texture types
        if (type_ == TextureType::ROUGHNESS || type_ == TextureType::METALNESS ||
            type_ == TextureType::AMBIENT_OCCLUSION) {
            if (nr_channels >= 3) {
                format = GL_RED; // Read only red channel from source
                internal_format = GL_R8; // Store as single channel
            }
        }

        // Upload to GPU
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        GLenum gl_error = glGetError();
        if (gl_error != GL_NO_ERROR) {
            std::cerr << "OpenGL error uploading texture: " << gl_error << std::endl;
            stbi_image_free(data);
            glDeleteTextures(1, &texture_id_);
            texture_id_ = 0;
            return;
        }

        // Generate mipmaps and set parameters
        if (settings_.generate_mipmaps) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, get_gl_wrap_mode(settings_.wrap_s));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, get_gl_wrap_mode(settings_.wrap_t));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, get_gl_filter_mode(settings_.min_filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, get_gl_filter_mode(settings_.mag_filter));

        // Clean up and mark as valid
        stbi_image_free(data);
        is_valid_ = true;

        // std::cout << "Successfully loaded texture: " << path_
        //         << " (" << width << "x" << height << ", " << nr_channels << " channels)" << std::endl;
    }

    bool Texture::is_valid() const {
        return texture_id_ != 0 && is_valid_ && width_ > 0 && height_ > 0;
    }

    Texture::Texture(const TextureHandle handle, const TextureType type, int width, int height, int channels) : texture_id_(handle), type_(type), width_(width), height_(height), nr_channels_(channels)  {}

    void Texture::bind(unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
    }

    void Texture::unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLint Texture::get_gl_wrap_mode(const TextureWrap wrap) const {
        switch (wrap) {
            case TextureWrap::REPEAT: return GL_REPEAT;
            case TextureWrap::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
            case TextureWrap::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
            case TextureWrap::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
            default: return GL_REPEAT;
        }
    }

    GLint Texture::get_gl_filter_mode(TextureFilter filter) const {
        switch (filter) {
            case TextureFilter::NEAREST: return GL_NEAREST;
            case TextureFilter::LINEAR: return GL_LINEAR;
            case TextureFilter::LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
            case TextureFilter::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
            default: return GL_LINEAR;
        }
    }
}
