#include "DCraft/Graphics/Primitives/Shape3D.h"

#include <glm/gtc/type_ptr.inl>

#include "DCraft/Graphics/Managers/TextureManager.h"

namespace DCraft {
    void Shape3D::set_texture(const std::string &path, TextureType type = TextureType::DIFFUSE) {
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

        std::vector<Texture *> texture_list = {texture_};

        // If we have a mesh, update its textures
        if (has_mesh()) {
            get_mesh()->update_textures(texture_list);
        }
    }

    void Shape3D::set_texture(Texture *texture, bool take_ownsership) {
        if (owns_texture_ && texture_) {
            delete texture_;
        }

        texture_ = texture;
        owns_texture_ = take_ownsership;
    }

    Texture *Shape3D::get_texture() const {
        return texture_;
    }

    bool Shape3D::has_texture() const {
        return owns_texture_;
    }

    void Shape3D::draw_self(const glm::mat4 &view, const glm::mat4 &projection, uint32_t shader_program) {
        glm::mat4 mvp = projection * view * get_world_matrix();

        // Create a uniform for the model view projection matrix
        unsigned int mvpLoc = glGetUniformLocation(shader_program, "MVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        if (has_mesh()) {
            get_mesh()->draw(shader_program);
        }
    }

    void Shape3D::set_vertices() {
    }

    void Shape3D::set_colors() {
    }

    void Shape3D::set_uvs() {
    }
}
