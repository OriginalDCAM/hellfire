//
// Created by denzel on 14/08/2025.
//

#pragma once
#include <cstdint>
#include <glm/detail/type_mat.hpp>

#include "RendererContext.h"
#include "hellfire/assets/AnimationSystem.h"
#include "hellfire/ecs/CameraComponent.h"
#include "../shader/Shader.h"

namespace hellfire {
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

        static void upload_lights_to_shader(Shader& shader, OGLRendererContext& renderer_context) {
            shader.set_light_counts(renderer_context.num_directional_lights, renderer_context.num_point_lights);

            // Upload directional lights
            for (int i = 0; i < renderer_context.num_directional_lights && i < 4; i++) {
                Entity* light_entity = renderer_context.directional_light_entities[i];
                if (light_entity) {
                    auto* light_component = light_entity->get_component<LightComponent>();
                    if (light_component) {
                        light_component->upload_to_shader(shader, i);
                    }
                }
            }

            // Upload point lights  
            for (int i = 0; i < renderer_context.num_point_lights && i < 8; i++) {
                Entity* light_entity = renderer_context.point_light_entities[i];
                if (light_entity) {
                    auto* light_component = light_entity->get_component<LightComponent>();
                    if (light_component) {
                        light_component->upload_to_shader(shader, i);
                    }
                }
            }

            // Upload camera position
            if (renderer_context.camera_component) {
                auto* camera_transform = renderer_context.camera_component->get_owner().get_component<TransformComponent>();
                if (camera_transform) {
                    shader.set_camera_position(camera_transform->get_world_position());
                }
            }
        }

        static bool is_valid_mesh(const std::shared_ptr<Mesh>& mesh) {
            return mesh && mesh->get_index_count() > 0;
        }
    };
}
