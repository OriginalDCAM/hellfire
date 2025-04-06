#pragma once
#include <string>
#include <vector>

#include "DCraft/Graphics/Textures/Texture.h"

namespace DCraft {
    class Material {
    public:
        Material(const std::string &name = "DefaultMaterial");

        void set_texture(const std::string &path, TextureType type);
        void set_texture(Texture *texture);

        const std::vector<Texture *> &get_textures() const;
        const std::string &get_name() const;

        float texture_repeat_x, texture_repeat_y;

        bool has_texture() const;
        void set_texture_repeat(float repeat_x, float repeat_y);

        void set_shader(uint32_t shader_id);
        uint32_t get_shader() const;
    private:
        std::string name;
        std::vector<Texture*> textures_;
        uint32_t shader_program_id;
    };
}
