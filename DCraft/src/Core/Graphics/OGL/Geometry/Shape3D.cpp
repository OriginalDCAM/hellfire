#include "DCraft/Graphics/Primitives/Shape3D.h"

#include "DCraft/Graphics/Managers/TextureManager.h"
#include "DCraft/Graphics/Materials/Material.h"
#include "DCraft/Graphics/Materials/MaterialRenderer.h"

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
        Mesh* mesh = get_mesh();
        return mesh ? mesh->get_material() : nullptr;
    }

    void Shape3D::draw_self(const glm::mat4 &view, const glm::mat4 &projection, uint32_t shader_program, void* renderer_context) {
        if (!has_mesh()) return;

        Mesh* mesh = get_mesh();
        if (!mesh) return;

        Material* material = mesh->get_material();
        if (!material) return;

        // Calculate matrices
        glm::mat4 model = get_world_matrix();
        glm::mat4 mvp = projection * view * model;

        // Determine which shader to use
        uint32_t actual_shader;
        if (material->get_compiled_shader_id() != 0) {
            actual_shader = material->get_compiled_shader_id();
        } else {
            actual_shader = shader_program;
        }

        // Bind the shader program
        glUseProgram(actual_shader);

        // Set standard uniforms that all shaders expect
        set_standard_uniforms(actual_shader, model, view, projection, mvp);

        MaterialRenderer::bind_material(*material);

        // Draw the mesh
        mesh->draw();
    }

    void Shape3D::set_standard_uniforms(uint32_t shader_program, 
                                   const glm::mat4& model, 
                                   const glm::mat4& view, 
                                   const glm::mat4& projection,
                                   const glm::mat4& mvp) {
        // Set transformation matrices
        GLint mvp_loc = glGetUniformLocation(shader_program, "MVP");
        if (mvp_loc != -1) {
            glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));
        }

        GLint model_loc = glGetUniformLocation(shader_program, "model");
        if (model_loc != -1) {
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        }

        GLint view_loc = glGetUniformLocation(shader_program, "view");
        if (view_loc != -1) {
            glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
        }

        GLint projection_loc = glGetUniformLocation(shader_program, "projection");
        if (projection_loc != -1) {
            glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));
        }

        // Set other common uniforms
        GLint normal_matrix_loc = glGetUniformLocation(shader_program, "normalMatrix");
        if (normal_matrix_loc != -1) {
            glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model)));
            glUniformMatrix3fv(normal_matrix_loc, 1, GL_FALSE, glm::value_ptr(normal_matrix));
        }

        // You might want to set other common uniforms here like:
        // - Camera position (viewPos)
        // - Time
        // - Screen resolution
        // etc.
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
