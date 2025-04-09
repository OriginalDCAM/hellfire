//
// Created by denzel on 04/04/2025.
//
#include "DCraft/Graphics/Materials/Material.h"
#include <unordered_map>

namespace DCraft {
    Material::Material(const std::string &name) : name(name) {
    }

    void Material::set_texture(const std::string &path, TextureType type) {
        Texture *texture = new Texture(path, type);
        textures_.push_back(texture);
        textures_map_[type].push_back(texture);
    }

    void Material::set_texture(Texture *texture) {
        textures_.push_back(texture);
        textures_map_[texture->get_type()].push_back(texture);
    }

    const std::vector<Texture *> &Material::get_textures() const {
        return textures_;
    }

    const std::string &Material::get_name() const {
        return name;
    }

    void Material::set_texture_repeat(float repeat_x, float repeat_y) {
        texture_repeat_x = repeat_x;
        texture_repeat_y = repeat_y;
    }

    std::tuple<float, float> Material::get_texture_repeat() {
        return {texture_repeat_x, texture_repeat_y };
    }

    void Material::set_shader(uint32_t shader_id) {
        shader_handle = shader_id;
    }

    uint32_t Material::get_shader() const {
        return shader_handle;
    }

    bool Material::has_custom_shader() const {
        return shader_handle != INVALID_SHADER;
    }
}
