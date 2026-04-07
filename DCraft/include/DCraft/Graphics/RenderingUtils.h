//
// Created by denzel on 14/08/2025.
//

#pragma once
#include <cstdint>
#include <glm/detail/type_mat.hpp>

#include "RendererContext.h"
#include "DCraft/Addons/AnimationSystem.h"
#include "DCraft/Components/CameraComponent.h"
#include "Shader/Shader.h"

namespace DCraft {
    /// Utility class containing shared rendering functionality.
    /// Used by composition in both RenderableComponent and InstancedRenderableComponent.
    class RenderingUtils {
    public:
        static void set_standard_uniforms(Shader &shader,
                                          const glm::mat4 &model,
                                          const glm::mat4 &view,
                                          const glm::mat4 &projection,
                                          float time = 0.0f) {
            shader.set_transform_matrices(model, view, projection);
            shader.set_time(time);
        }

        static void set_view_projection_uniforms(Shader &shader,
                                                 const glm::mat4 &view,
                                                 const glm::mat4 &projection,
                                                 float time = 0.0f) {
            shader.set_view_projection(view, projection);
            shader.set_time(time);
        }

        static void upload_lights_to_shader(Shader& shader, void* renderer_context) {
            if (!renderer_context) return;

            OGLRendererContext* ogl_context = static_cast<OGLRendererContext*>(renderer_context);
            shader.set_light_counts(ogl_context->num_directional_lights, ogl_context->num_point_lights);

            // Upload directional lights
            for (int i = 0; i < ogl_context->num_directional_lights && i < 4; i++) {
                Entity* light_entity = ogl_context->directional_light_entities[i];
                if (light_entity) {
                    auto* light_component = light_entity->get_component<LightComponent>();
                    if (light_component) {
                        light_component->upload_to_shader(shader, i);
                    }
                }
            }

            // Upload point lights  
            for (int i = 0; i < ogl_context->num_point_lights && i < 8; i++) {
                Entity* light_entity = ogl_context->point_light_entities[i];
                if (light_entity) {
                    auto* light_component = light_entity->get_component<LightComponent>();
                    if (light_component) {
                        light_component->upload_to_shader(shader, i);
                    }
                }
            }

            // Upload camera position
            if (ogl_context->camera_component) {
                auto* camera_transform = ogl_context->camera_component->get_owner()->get_component<TransformComponent>();
                if (camera_transform) {
                    shader.set_camera_position(camera_transform->get_world_position());
                }
            }
        }

        static uint32_t get_shader_to_use(Mesh *mesh, uint32_t fallback_shader) {
            if (!mesh) return fallback_shader;

            Material *material = mesh->get_material();
            if (!material) return fallback_shader;

            return material->get_compiled_shader_id() != 0
                       ? material->get_compiled_shader_id()
                       : fallback_shader;
        }

        // Common mesh validation
        static bool validate_mesh_for_rendering(Mesh *mesh) {
            return mesh && mesh->get_material();
        }
    };
}
