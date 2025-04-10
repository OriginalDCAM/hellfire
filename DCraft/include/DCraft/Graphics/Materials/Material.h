#pragma once
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include "DCraft/Graphics/Textures/Texture.h"

namespace DCraft {
    class Material {
    protected:
        std::string name;
        std::vector<Texture *> textures_;
        std::unordered_map<TextureType, std::vector<Texture *> > textures_map_;
        float texture_repeat_x = 1.0f;
        float texture_repeat_y = 1.0f;

        using ShaderHandle = uint64_t;
        static constexpr ShaderHandle INVALID_SHADER = std::numeric_limits<ShaderHandle>::max();
        ShaderHandle shader_handle = INVALID_SHADER;
        bool has_diffuse_texture_ = false;



        virtual void on_diffuse_texture_set(Texture* texture) {}
        virtual void on_specular_texture_set(Texture* texture) {}

    public:
        Material(const std::string &name);

        virtual ~Material() = default;

        void set_diffuse_texture(std::string& path) { set_texture(path, TextureType::DIFFUSE); }
        void set_diffuse_texture(Texture* texture) {
            set_texture(texture);
            has_diffuse_texture_ = true;
        }
        Texture* get_diffuse_texture() {
            auto& diffuseTextures = textures_map_[TextureType::DIFFUSE];
            return diffuseTextures.empty() ? nullptr : diffuseTextures[0];
        }

        void set_texture(const std::string &path, TextureType type);

        void set_texture(Texture *texture);

        const std::vector<Texture *> &get_textures() const;

        const std::string &get_name() const;

        bool has_texture() const;

        void set_texture_repeat(float repeat_x, float repeat_y);

        std::tuple<float, float> get_texture_repeat();

        void set_shader(uint32_t shader_id);

        uint32_t get_shader() const;

        bool has_custom_shader() const;

        bool has_diffuse_texture() const { return has_diffuse_texture_; }

        virtual void bind(void *renderer_context) = 0;

        void set_name(const std::string& value) { name = value; }

    };
}
