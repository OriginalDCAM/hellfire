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

    public:
        Material(const std::string &name);

        virtual ~Material() = default;

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

        virtual void bind(void *renderer_context) = 0;
    };
}
