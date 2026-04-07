//
// Created by denzel on 08/08/2025.
//

#pragma once
#include <glm/detail/type_vec.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TransformComponent.h"
#include "hellfire/graphics/shader/Shader.h"
#include "Component.h"
#include "Entity.h"
#include "GL/glew.h"

namespace hellfire {
    class LightComponent : public Component {
    public:
        enum LightType {
            DIRECTIONAL,
            SPOT,
            POINT
        };

    private:
        // Common properties
        LightType type_ = DIRECTIONAL;
        glm::vec3 color_ = glm::vec3(1.0f);
        float intensity_ = 1.0f;

        // Directional light properties
        glm::vec3 direction_ = glm::vec3(0.0f, -1.0f, 0.0f);

        // Point light properties
        float range_ = 10.0f;
        float attenuation_ = 1.0f;

        // Spotlight properties
        float inner_cone_angle_ = 30.0f;
        float outer_cone_angle_ = 45.0f;

    public:
        LightComponent(LightType type = LightType::DIRECTIONAL) : type_(type) {
        }

        // Type management
        LightType get_light_type() const { return type_; }
        void set_light_type(LightType type) { type_ = type; }

        // Common properties
        void set_color(const glm::vec3 &color) { color_ = color; }
        const glm::vec3 &get_color() const { return color_; }

        void set_intensity(const float intensity) { intensity_ = intensity; }
        const float &get_intensity() const { return intensity_; }

        // Directional light methods
        void set_direction(float x, float y, float z) {
            set_direction(glm::vec3(x, y, z));
        }

        void set_direction(const glm::vec3 &direction) {
            direction_ = glm::normalize(direction);
        }

        const glm::vec3& get_direction() const { return direction_; }

        void look_at(float x, float y, float z) {
            look_at(glm::vec3(x, y, z));
        }

        void look_at(const glm::vec3 &target) {
            auto *transform = get_owner()->get_component<TransformComponent>();
            
            if (transform) {
                glm::vec3 position = transform->get_position();
                glm::vec3 new_direction = glm::normalize(target - position);
                set_direction(new_direction);
            }
        }

        // Point light methods
        void set_range(float range) { range_ = range; }
        float get_range() const { return range_; }

        void set_attenuation(float attenuation) { attenuation_ = attenuation; }
        float get_attenuation() const { return attenuation_; }

        // Spotlight methods
        void set_inner_cone_angle(float angle) { inner_cone_angle_ = angle; }
        float get_inner_cone_angle() const { return inner_cone_angle_; }

        void set_outer_cone_angle(float angle) { outer_cone_angle_ = angle; }
        float get_outer_cone_angle() const { return outer_cone_angle_; }

        // Shader upload method
        void upload_to_shader(Shader& shader, int light_index) const {
            switch (type_) {
                case DIRECTIONAL:
                    upload_directional_to_shader(shader, light_index);
                    break;
                case POINT:
                    upload_point_to_shader(shader, light_index);
                    break;
                case SPOT:
                    upload_spot_to_shader(shader, light_index);
                    break;
            }
        }

        // Factory methods for convenience
        static LightComponent *create_directional(const glm::vec3 &direction = glm::vec3(0.0f, -1.0f, 0.0f)) {
            auto *light = new LightComponent(DIRECTIONAL);
            light->set_direction(direction);
            return light;
        }

        static LightComponent *create_point(float range = 10.0f, float attenuation = 1.0f) {
            auto *light = new LightComponent(POINT);
            light->set_range(range);
            light->set_attenuation(attenuation);
            return light;
        }

        static LightComponent *create_spot(float inner_angle = 30.0f, float outer_angle = 45.0f) {
            auto *light = new LightComponent(SPOT);
            light->set_inner_cone_angle(inner_angle);
            light->set_outer_cone_angle(outer_angle);
            return light;
        }

    private:
        void upload_directional_to_shader(Shader& shader, int light_index) const {
            shader.set_directional_light(light_index, direction_, color_, intensity_);
        }

        void upload_point_to_shader(Shader& shader, int light_index) const {

            // Get world position from transform
            glm::vec3 position(0.0f);
            if (get_owner()) {
                auto *transform = get_owner()->get_component<TransformComponent>();
                if (transform) {
                    position = transform->get_position();
                }
            }

            shader.set_point_light(light_index, position, color_, intensity_, range_, attenuation_);
        }

        void upload_spot_to_shader(Shader& shader, int light_index) const {
            // TODO: Actually support spotlights in shader
            // std::string base = "spotLights[" + std::to_string(light_index) + "]";

            // Get world position and direction from transform
            // glm::vec3 position(0.0f);
            // glm::vec3 world_direction = direction_;

            // if (get_owner()) {
            //     auto *transform = get_owner()->get_component<TransformComponent>();
            //     if (transform) {
            //         position = transform->get_position();
            //         // Transform direction by entity's rotation
            //         glm::mat4 rotation = transform->get_rotation_matrix();
            //         world_direction = glm::mat3(rotation) * direction_;
            //     }
            // }
            // glUniform3fv(glGetUniformLocation(shader_program, (base + ".position").c_str()), 1,
            //              glm::value_ptr(position));
            // glUniform3fv(glGetUniformLocation(shader_program, (base + ".direction").c_str()), 1,
            //              glm::value_ptr(world_direction));
            // glUniform3fv(glGetUniformLocation(shader_program, (base + ".color").c_str()), 1,
            //              glm::value_ptr(color_));
            // glUniform1f(glGetUniformLocation(shader_program, (base + ".intensity").c_str()), intensity_);
            // glUniform1f(glGetUniformLocation(shader_program, (base + ".range").c_str()), range_);
            // glUniform1f(glGetUniformLocation(shader_program, (base + ".innerConeAngle").c_str()),
            //             glm::cos(glm::radians(inner_cone_angle_)));
            // glUniform1f(glGetUniformLocation(shader_program, (base + ".outerConeAngle").c_str()),
            //             glm::cos(glm::radians(outer_cone_angle_)));
        }
    };
}
