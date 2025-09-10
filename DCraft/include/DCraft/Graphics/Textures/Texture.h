#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

#include "GL/glew.h"

namespace DCraft {
    class Texture;
    class Material;

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
        TextureFilter min_filter = TextureFilter::LINEAR_MIPMAP_LINEAR;
        TextureFilter mag_filter = TextureFilter::LINEAR;
        TextureWrap wrap_s = TextureWrap::REPEAT;
        TextureWrap wrap_t = TextureWrap::REPEAT;
        bool generate_mipmaps = true;
        bool flip_vertically = true;

        static TextureSettings for_type(TextureType type);
    };


    class MaterialTextureSet {
    public:
        // Fluent interface for building texture sets
        MaterialTextureSet &diffuse(const std::string &path);

        MaterialTextureSet &normal(const std::string &path);

        MaterialTextureSet &specular(const std::string &path);

        MaterialTextureSet &roughness(const std::string &path);

        MaterialTextureSet &metalness(const std::string &path);

        MaterialTextureSet &ao(const std::string &path);

        MaterialTextureSet &emissive(const std::string &path);

        // Add texture with custom type
        MaterialTextureSet &texture(TextureType type, const std::string &path);

        // Getters
        std::shared_ptr<Texture> get(TextureType type) const;

        bool has(TextureType type) const;

        // Apply to material
        void apply_to_material(Material &material) const;

        // Bind all textures for rendering
        void bind_all() const;

        // Load from directory (auto-detect based on naming conventions)
        static MaterialTextureSet from_directory(const std::string &base_path, const std::string &material_name);

    private:
        std::unordered_map<TextureType, std::shared_ptr<Texture> > textures_;
    };

    class Texture {
    public:
        int width, height, nr_channels;

        Texture(const std::string &path, TextureType type = TextureType::DIFFUSE);

        Texture(const std::string &path, TextureType type, const TextureSettings &settings);

        Texture(const Texture &) = delete;
    
        Texture &operator=(const Texture &) = delete;

        Texture(Texture &&other) noexcept;

        Texture &operator=(Texture &&other) noexcept;

        ~Texture();

        void bind(unsigned int slot = 0) const;

        void unbind() const;

        TextureType get_type() const { return type_; }
        uint32_t get_id() { return texture_id_; }
        const std::string &get_path() { return path_; }
        
        int get_slot() { return slot_ ; }
        void set_slot(int slot) { slot_ = slot; }
        
        bool is_valid() const { return texture_id_ != 0; }

        // Texture parameters
        void set_wrap_mode(TextureWrap wrap_s, TextureWrap wrap_t);

        void set_filter_mode(TextureFilter min_filter, TextureFilter mag_filter);

        // Utility methods
        static std::string type_to_string(TextureType type);

        static std::string get_uniform_name(TextureType type);

    private:
        TextureType type_;
        std::string path_;
        uint32_t texture_id_;
        TextureSettings settings_;
        int slot_ = 0;

        void load_texture_data();

        GLenum get_gl_wrap_mode(TextureWrap wrap) const;

        GLenum get_gl_filter_mode(TextureFilter filter) const;
    };

    class TextureCache {
    public:
        static std::shared_ptr<Texture> load(const std::string &path, TextureType type = TextureType::DIFFUSE,
                                             const TextureSettings &settings = TextureSettings{});

        static void clear_cache();

        static size_t get_cache_size();

    private:
        static std::unordered_map<std::string, std::weak_ptr<Texture> > cache_;
    };
}
