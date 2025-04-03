#include "DCraft/Graphics/Primitives/Shape.h"

#include "DCraft/Graphics/Managers/TextureManager.h"

namespace DCraft {
    void Shape::set_texture(const std::string &path, TextureType type = TextureType::DIFFUSE) {
        Texture *texture = TextureManager::get_instance().get_texture(path);
        if (texture) {
            set_texture(texture, false);
        } else {
            texture = new Texture(path);
            TextureManager::get_instance().add_texture(path, texture);
            set_texture(texture, false);
        }

        // Store the texture in the Shape
        texture_ = texture;
        texture_->type = type;

        std::vector texture_list = {texture_};

        for (Mesh &mesh: get_model()->meshes) {
            mesh.update_textures(texture_list);
        }
    }

    void Shape::set_texture(Texture *texture, bool take_ownsership) {
        if (owns_texture_ && texture_) {
            delete texture_;
        }

        texture_ = texture;
        owns_texture_ = take_ownsership;
    }

    Texture *Shape::get_texture() const {
        return texture_;
    }

    bool Shape::has_texture() const {
        return owns_texture_;
    }

    void Shape::set_vertices() {
    }

    void Shape::set_colors() {
    }

    void Shape::set_uvs() {
    }
}
