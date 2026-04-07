#include "DCraft/Graphics/Textures/Texture.h"

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <fstream>
#include <iostream>
#include <set>
#include <stb/stb_image.h>

#include "assimp/code/Common/Win32DebugLogStream.h"
#include "DCraft/Graphics/Materials/Material.h"
#include "DCraft/Utility/TextureUtils.h"

namespace hellfire {
    // Static cache for TextureCache
    std::unordered_map<std::string, std::weak_ptr<Texture> > TextureCache::cache_;

    TextureSettings TextureSettings::for_type(TextureType type) {
        TextureSettings settings;

        settings.max_size = 1024;
        switch (type) {
            case TextureType::NORMAL:
                settings.flip_vertically = true;
                break;
            case TextureType::ROUGHNESS:
            case TextureType::METALNESS:
            case TextureType::AMBIENT_OCCLUSION:
                settings.min_filter = TextureFilter::LINEAR; // Single channel, less filtering
                break;
            case TextureType::DIFFUSE:
            case TextureType::EMISSIVE:
                settings.generate_mipmaps = true; // Full quality for color textures
                break;
            default:
                break;
        }

        return settings;
    }

    Texture::Texture(const std::string &path, TextureType type)
        : Texture(path, type, TextureSettings::for_type(type)) {
    }

    Texture::Texture(const std::string &path, TextureType type, const TextureSettings &settings)
        : path_(path), type_(type), settings_(settings) {
        load_texture_data();
    }

    Texture::Texture(Texture &&other) noexcept
        : width(other.width), height(other.height), nr_channels(other.nr_channels),
          type_(other.type_), path_(std::move(other.path_)),
          texture_id_(other.texture_id_), settings_(other.settings_) {
        other.texture_id_ = 0; // Transfer ownership
    }

    Texture &Texture::operator=(Texture &&other) noexcept {
        if (this != &other) {
            // Clean up current texture
            if (texture_id_ != 0) {
                glDeleteTextures(1, &texture_id_);
            }

            // Transfer ownership
            width = other.width;
            height = other.height;
            nr_channels = other.nr_channels;
            type_ = other.type_;
            path_ = std::move(other.path_);
            texture_id_ = other.texture_id_;
            settings_ = other.settings_;

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

        // Downscale texture if needed
        // if (settings_.max_size > 0 && (width > settings_.max_size || height > settings_.max_size)) {
        //     const int new_width = std::min(width, settings_.max_size);
        //     const int new_height = std::min(height, settings_.max_size);
        //
        //     const bool use_srgb = (type_ != TextureType::NORMAL);
        //
        //     unsigned char *resized_data = resize_image(data, width, height, new_width, new_height, desired_channels,
        //                                                use_srgb);
        //
        //     stbi_image_free(data);
        //     data = resized_data;
        //     width = new_width;
        //     height = new_height;
        //
        //     std::cout << "Downscaled texture from " << width << "x" << height << " to " << new_width << "x" <<
        //             new_height << std::endl;
        // }

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

        std::cout << "Successfully loaded texture: " << path_
                << " (" << width << "x" << height << ", " << nr_channels << " channels)" << std::endl;
    }

    bool Texture::is_valid() const {
        return texture_id_ != 0 && is_valid_ && width > 0 && height > 0;
    }

    void Texture::bind(unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture_id_);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL error in texture bind: " << error << std::endl;
        }
    }

    void Texture::unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    std::string Texture::type_to_string(TextureType type) {
        switch (type) {
            case TextureType::DIFFUSE: return "Diffuse";
            case TextureType::SPECULAR: return "Specular";
            case TextureType::NORMAL: return "Normal";
            case TextureType::AMBIENT_OCCLUSION: return "AO";
            case TextureType::ROUGHNESS: return "Roughness";
            case TextureType::METALNESS: return "Metalness";
            case TextureType::EMISSIVE: return "Emissive";
            case TextureType::HEIGHT: return "Height";
            case TextureType::OPACITY: return "Opacity";
            default: return "Unknown";
        }
    }

    std::string Texture::get_uniform_name(TextureType type) {
        switch (type) {
            case TextureType::DIFFUSE: return "uDiffuseTexture";
            case TextureType::SPECULAR: return "uSpecularTexture";
            case TextureType::NORMAL: return "uNormalTexture";
            case TextureType::AMBIENT_OCCLUSION: return "uAOTexture";
            case TextureType::ROUGHNESS: return "uRoughnessTexture";
            case TextureType::METALNESS: return "uMetalnessTexture";
            case TextureType::EMISSIVE: return "uEmissiveTexture";
            case TextureType::HEIGHT: return "uHeightTexture";
            case TextureType::OPACITY: return "uOpacityTexture";
            default: return "uTexture";
        }
    }

    GLenum Texture::get_gl_wrap_mode(TextureWrap wrap) const {
        switch (wrap) {
            case TextureWrap::REPEAT: return GL_REPEAT;
            case TextureWrap::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
            case TextureWrap::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
            case TextureWrap::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
            default: return GL_REPEAT;
        }
    }

    GLenum Texture::get_gl_filter_mode(TextureFilter filter) const {
        switch (filter) {
            case TextureFilter::NEAREST: return GL_NEAREST;
            case TextureFilter::LINEAR: return GL_LINEAR;
            case TextureFilter::LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
            case TextureFilter::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
            default: return GL_LINEAR;
        }
    }

    std::shared_ptr<Texture> TextureCache::load(const std::string &path, TextureType type,
                                                const TextureSettings &settings) {
        std::string cache_key = path + "_" + std::to_string(static_cast<int>(type));
        auto it = cache_.find(cache_key);
        if (it != cache_.end()) {
            auto shared_texture = it->second.lock();
            if (shared_texture) {
                return shared_texture;
            } else {
                cache_.erase(it);
            }
        }

        // Create new texture
        auto texture = std::make_shared<Texture>(path, type);
        if (texture->is_valid()) {
            cache_[cache_key] = texture;
            return texture;
        } else {
            std::cerr << "Failed to create valid texture from: " << path << std::endl;
            return nullptr;
        }
    }

    void TextureCache::clear_cache() {
        cache_.clear();
    }

    size_t TextureCache::get_cache_size() {
        for (auto it = cache_.begin(); it != cache_.end();) {
            if (it->second.expired()) {
                it = cache_.erase(it);
            } else {
                ++it;
            }
        }
        return cache_.size();
    }

    MaterialTextureSet &MaterialTextureSet::diffuse(const std::string &path) {
        textures_[TextureType::DIFFUSE] = TextureCache::load(path, TextureType::DIFFUSE);
        return *this;
    }

    MaterialTextureSet &MaterialTextureSet::normal(const std::string &path) {
        textures_[TextureType::NORMAL] = TextureCache::load(path, TextureType::NORMAL);
        return *this;
    }

    MaterialTextureSet &MaterialTextureSet::specular(const std::string &path) {
        textures_[TextureType::SPECULAR] = TextureCache::load(path, TextureType::SPECULAR);
        return *this;
    }

    MaterialTextureSet &MaterialTextureSet::roughness(const std::string &path) {
        textures_[TextureType::ROUGHNESS] = TextureCache::load(path, TextureType::ROUGHNESS);
        return *this;
    }

    MaterialTextureSet &MaterialTextureSet::metalness(const std::string &path) {
        textures_[TextureType::METALNESS] = TextureCache::load(path, TextureType::METALNESS);
        return *this;
    }

    MaterialTextureSet &MaterialTextureSet::texture(TextureType type, const std::string &path) {
        textures_[type] = TextureCache::load(path, type);
        return *this;
    }

    MaterialTextureSet &MaterialTextureSet::ao(const std::string &path) {
        textures_[TextureType::AMBIENT_OCCLUSION] = TextureCache::load(path, TextureType::AMBIENT_OCCLUSION);
        return *this;
    }

    MaterialTextureSet &MaterialTextureSet::emissive(const std::string &path) {
        textures_[TextureType::EMISSIVE] = TextureCache::load(path, TextureType::EMISSIVE);
        return *this;
    }

    std::shared_ptr<Texture> MaterialTextureSet::get(TextureType type) const {
        auto it = textures_.find(type);
        return (it != textures_.end()) ? it->second : nullptr;
    }

    bool MaterialTextureSet::has(TextureType type) const {
        auto it = textures_.find(type);
        return (it != textures_.end()) && (it->second != nullptr) && (it->second->is_valid());
    }

    void MaterialTextureSet::bind_all() const {
        int texture_unit = 0;
        for (const auto &[type, texture]: textures_) {
            if (texture && texture->is_valid()) {
                texture->bind(texture_unit);
                texture_unit++;
            }
        }
    }

    void MaterialTextureSet::apply_to_material(Material &material) const {
        for (const auto &[type, texture]: textures_) {
            if (texture && texture->is_valid()) {
                std::string uniform_name = Texture::get_uniform_name(type);
                material.set_property(uniform_name, texture.get(), uniform_name);
            }
        }
    }

    bool file_exists(const std::string &filename) {
        return std::filesystem::exists(filename);
    }

    MaterialTextureSet MaterialTextureSet::from_directory(const std::string &base_path,
                                                          const std::string &material_name) {
        MaterialTextureSet texture_set;

        // Common naming conventions for different texture types
        std::vector<std::pair<TextureType, std::vector<std::string> > > naming_patterns = {
            {
                TextureType::DIFFUSE, {
                    material_name + "_diffuse.jpg", material_name + "_diffuse.png",
                    material_name + "_albedo.jpg", material_name + "_albedo.png",
                    material_name + "_color.jpg", material_name + "_color.png",
                    material_name + "_basecolor.jpg", material_name + "_basecolor.png"
                }
            },
            {
                TextureType::NORMAL, {
                    material_name + "_normal.jpg", material_name + "_normal.png",
                    material_name + "_nrm.jpg", material_name + "_nrm.png",
                    material_name + "_norm.jpg", material_name + "_norm.png",
                    material_name + "_normalmap.jpg", material_name + "_normalmap.png"
                }
            },
            {
                TextureType::SPECULAR, {
                    material_name + "_specular.jpg", material_name + "_specular.png",
                    material_name + "_spec.jpg", material_name + "_spec.png",
                    material_name + "_gloss.jpg", material_name + "_gloss.png"
                }
            },
            {
                TextureType::ROUGHNESS, {
                    material_name + "_roughness.jpg", material_name + "_roughness.png",
                    material_name + "_rough.jpg", material_name + "_rough.png"
                }
            },
            {
                TextureType::METALNESS, {
                    material_name + "_metalness.jpg", material_name + "_metalness.png",
                    material_name + "_metal.jpg", material_name + "_metal.png",
                    material_name + "_metallic.jpg", material_name + "_metallic.png"
                }
            },
            {
                TextureType::AMBIENT_OCCLUSION, {
                    material_name + "_ao.jpg", material_name + "_ao.png",
                    material_name + "_occlusion.jpg", material_name + "_occlusion.png",
                    material_name + "_ambient.jpg", material_name + "_ambient.png"
                }
            },
            {
                TextureType::EMISSIVE, {
                    material_name + "_emissive.jpg", material_name + "_emissive.png",
                    material_name + "_emission.jpg", material_name + "_emission.png",
                    material_name + "_glow.jpg", material_name + "_glow.png"
                }
            },
            {
                TextureType::HEIGHT, {
                    material_name + "_height.jpg", material_name + "_height.png",
                    material_name + "_displacement.jpg", material_name + "_displacement.png",
                    material_name + "_disp.jpg", material_name + "_disp.png"
                }
            },
            {
                TextureType::OPACITY, {
                    material_name + "_opacity.jpg", material_name + "_opacity.png",
                    material_name + "_alpha.jpg", material_name + "_alpha.png",
                    material_name + "_mask.jpg", material_name + "_mask.png"
                }
            }
        };

        // Try to find textures based on naming patterns
        for (const auto &[type, patterns]: naming_patterns) {
            for (const auto &pattern: patterns) {
                std::string full_path = base_path;

                // Ensure proper path separator
                if (!base_path.empty() && base_path.back() != '/' && base_path.back() != '\\') {
                    full_path += "/";
                }
                full_path += pattern;

                if (file_exists(full_path)) {
                    // Found one, move to next type
                    texture_set.texture(type, full_path);
                    break;
                }
            }
        }

        return texture_set;
    }
}
