#include "DCraft/Graphics/Primitives/MeshRenderer.h"

#include "DCraft/Graphics/Renderer.h"
#include "DCraft/Graphics/Managers/TextureManager.h"
#include "DCraft/Graphics/Materials/Material.h"
#include "DCraft/Graphics/Materials/MaterialRenderer.h"

namespace DCraft {
    Texture *MeshRenderer::get_texture() const {
        return texture_;
    }

    bool MeshRenderer::has_texture() const {
        return owns_texture_;
    }

    void MeshRenderer::set_material(Material *material) {
        if (material) {
            if (has_mesh()) {
                get_mesh()->set_material(material);
            }
        }
    }

    Material *MeshRenderer::get_material() const {
        Mesh* mesh = get_mesh();
        return mesh ? mesh->get_material() : nullptr;
    }

    void MeshRenderer::draw_self(const glm::mat4 &view, const glm::mat4 &projection, uint32_t shader_program, void* renderer_context) {

        
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

        if (renderer_context) {
            upload_lights_to_current_shader(actual_shader, renderer_context);
        }

        // Set standard uniforms that all shaders expect
        set_standard_uniforms(actual_shader, model, view, projection, mvp);

        MaterialRenderer::bind_material(*material);

        // Draw the mesh
        mesh->draw();
    }

    void MeshRenderer::set_standard_uniforms(uint32_t shader_program, 
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

    }

    void MeshRenderer::upload_lights_to_current_shader(uint32_t shader_program, void* renderer_context) {
        if (!renderer_context) return;
        
        // Cast to get the renderer context with lighting data
        OGLRendererContext* ogl_context = static_cast<OGLRendererContext*>(renderer_context);

        // Upload light counts
        GLint dir_count_loc = glGetUniformLocation(shader_program, "numDirectionalLights");
        if (dir_count_loc != -1) {
            glUniform1i(dir_count_loc, ogl_context->num_directional_lights);
        }

        GLint point_count_loc = glGetUniformLocation(shader_program, "numPointLights");
        if (point_count_loc != -1) {
            glUniform1i(point_count_loc, ogl_context->num_point_lights);
        }

        // Upload directional lights
        for (int i = 0; i < ogl_context->num_directional_lights && i < 4; i++) {
            if (ogl_context->directional_lights[i]) {
                ogl_context->directional_lights[i]->upload_to_shader(shader_program, i);
            }
        }

        // Upload point lights  
        for (int i = 0; i < ogl_context->num_point_lights && i < 8; i++) {
            if (ogl_context->point_lights[i]) {
                ogl_context->point_lights[i]->upload_to_shader(shader_program, i);
            }
        }

        // Upload camera position (viewPos)
        if (ogl_context->camera) {
            GLint view_pos_loc = glGetUniformLocation(shader_program, "viewPos");
            if (view_pos_loc != -1) {
                glm::vec3 cam_pos = ogl_context->camera->get_position();
                glUniform3fv(view_pos_loc, 1, glm::value_ptr(cam_pos));
            }
        }
    }

    json MeshRenderer::to_json() {
        json j = Object3D::to_json();
        j["type"] = "Shape3D";
        return j;
    }

    void MeshRenderer::set_vertices() {
    }

    void MeshRenderer::set_colors() {
    }

    void MeshRenderer::set_uvs() {
    }
}
