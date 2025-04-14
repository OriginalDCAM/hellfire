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
    

    Material *Shape3D::get_material() const {
        return get_mesh()->get_material();
    }

    void Shape3D::draw_self(const glm::mat4 &view, const glm::mat4 &projection, uint32_t shader_program, void* renderer_context = nullptr) {
        if (!has_mesh()) return;

        Material* material = get_mesh()->get_material();
        if (!material) return;

        glm::mat4 model = get_world_matrix();
        glm::mat4 mvp = projection * view * model;

        uint32_t actual_shader;
        if (material->has_custom_shader()) {
            actual_shader = material->get_shader();
        } else {
            actual_shader = shader_program;
        }

        glUniformMatrix4fv(glGetUniformLocation(actual_shader, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(glGetUniformLocation(actual_shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

        material->bind(renderer_context);

        get_mesh()->draw();
    }

    json Shape3D::to_json() {
        json j = Object3D::to_json();
        j["type"] = "Shape3D";
        return j;
    }

    void Shape3D::set_vertices() {
    }

    void Shape3D::set_colors() {
    }

    void Shape3D::set_uvs() {
    }
}
