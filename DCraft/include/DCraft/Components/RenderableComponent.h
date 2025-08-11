#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "CameraComponent.h"
#include "DCraft/Structs/Component.h"
#include "DCraft/Components/TransformComponent.h"
#include "DCraft/Components/LightComponent.h"
#include "DCraft/Graphics/Mesh.h"
#include "DCraft/Graphics/Materials/MaterialRenderer.h"
#include "DCraft/Graphics/RendererContext.h"  // Include the separate context header

namespace DCraft {
    class RenderableComponent : public Component {
    public:
        RenderableComponent() = default;

        RenderableComponent(std::shared_ptr<Mesh> mesh) : mesh_(mesh) {
        }

        // Mesh management
        void set_mesh(std::shared_ptr<Mesh> mesh) { mesh_ = mesh; }
        void set_mesh(Mesh* mesh) { 
            if (mesh) {
                mesh_ = std::shared_ptr<Mesh>(mesh);
            } else {
                mesh_.reset();
            }
        }
        Mesh* get_mesh() const { return mesh_.get(); }
        bool has_mesh() const { return mesh_ != nullptr; }

        // Material convenience methods
        void set_material(Material* material) {
            if (material && has_mesh()) {
                get_mesh()->set_material(material);
            }
        }

        Material* get_material() const {
            return has_mesh() ? get_mesh()->get_material() : nullptr;
        }

        // Main rendering method
        void draw(const glm::mat4& view, const glm::mat4& projection, uint32_t shader_program,
                  void* renderer_context = nullptr) {
            // Fixed: Changed != to == in the condition
            if (!has_mesh() || !get_owner()) return;

            Mesh* mesh = get_mesh();
            if (!mesh) return;

            Material* material = mesh->get_material();
            if (!material) return;

            // Get transform from the entity
            auto* transform = get_owner()->get_component<TransformComponent>();
            if (!transform) return;

            // Calculate matrices
            glm::mat4 model = transform->get_world_matrix();
            glm::mat4 mvp = projection * view * model;

            // Determine which shader to use
            uint32_t shader_id = material->get_compiled_shader_id() != 0
                                     ? material->get_compiled_shader_id()
                                     : shader_program;

            glUseProgram(shader_id);

            if (renderer_context) {
                // Do lighting
                upload_lights_to_current_shader(shader_id, renderer_context);
            }

            // Set the standard uniforms
            set_standard_uniforms(shader_id, model, view, projection, mvp);

            MaterialRenderer::bind_material(*material);

            // Draw the mesh
            mesh->draw();
        }

    private:
        std::shared_ptr<Mesh> mesh_;

        void set_standard_uniforms(uint32_t shader_program,
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

        void upload_lights_to_current_shader(uint32_t shader_program, void* renderer_context) {
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
                Entity* light_entity = ogl_context->directional_light_entities[i];
                if (light_entity) {
                    auto* light_component = light_entity->get_component<LightComponent>();
                    if (light_component) {
                        light_component->upload_to_shader(shader_program, i);
                    }
                }
            }

            // Upload point lights  
            for (int i = 0; i < ogl_context->num_point_lights && i < 8; i++) {
                Entity* light_entity = ogl_context->point_light_entities[i];
                if (light_entity) {
                    auto* light_component = light_entity->get_component<LightComponent>();
                    if (light_component) {
                        light_component->upload_to_shader(shader_program, i);
                    }
                }
            }

            // Upload camera position (viewPos)
            if (ogl_context->camera_component) {
                GLint view_pos_loc = glGetUniformLocation(shader_program, "viewPos");
                if (view_pos_loc != -1) {
                    auto* camera_transform = ogl_context->camera_component->get_owner()->get_component<TransformComponent>();
                    if (camera_transform) {
                        glm::vec3 cam_pos = camera_transform->get_world_position();
                        glUniform3fv(view_pos_loc, 1, glm::value_ptr(cam_pos));
                    }
                }
            }
        }
    };
}