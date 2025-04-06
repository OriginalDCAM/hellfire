#include "DCraft/Graphics/Primitives/Shape3D.h"

#include <glm/gtc/type_ptr.inl>

#include "DCraft/Graphics/Managers/TextureManager.h"

namespace DCraft {
    Texture *Shape3D::get_texture() const {
        return texture_;
    }

    bool Shape3D::has_texture() const {
        return owns_texture_;
    }

    void Shape3D::set_material(Material *material) {
        if (material) {
            if (has_mesh()) {
                get_mesh()->set_material(material);
            }

        }
    }

    void Shape3D::draw_self(const glm::mat4 &view, const glm::mat4 &projection, uint32_t shader_program) {
        glm::mat4 mvp = projection * view * get_world_matrix();

        // Create a uniform for the model view projection matrix
        unsigned int mvpLoc = glGetUniformLocation(shader_program, "MVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        if (has_mesh()) {
            get_mesh()->draw();
        }
    }

    void Shape3D::set_vertices() {
    }

    void Shape3D::set_colors() {
    }

    void Shape3D::set_uvs() {
    }
}
