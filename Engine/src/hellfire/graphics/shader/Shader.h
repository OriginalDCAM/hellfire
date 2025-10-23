#pragma once
#include <cstdint>
#include <string>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderUniformBinder.h"
#include "hellfire/graphics/managers/ShaderManager.h"

class Shader {
public:
    explicit Shader(const uint32_t program_id) : program_id_(program_id) {
    }

    Shader() : program_id_(0) {
    }

    // Disable copy, enable move
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    Shader(Shader &&other) noexcept : program_id_(other.program_id_) {
        other.program_id_ = 0;
    }

    Shader &operator=(Shader &&other) noexcept {
        if (this != &other) {
            program_id_ = other.program_id_;
            other.program_id_ = 0;
        }
        return *this;
    }

    static Shader from_id(const uint32_t shader_id) {
        return Shader{shader_id};
    }

    // Core Methods
    void use() const {
        if (program_id_ != 0) {
            glUseProgram(program_id_);
        }
    }

    uint32_t get_program_id() const { return program_id_; }
    bool is_valid() const { return program_id_ != 0; }

    // === UNIFORM SETTERS ===
    void set_bool(const std::string &name, const bool value) const {
        get_binder().set_bool(name, value);
    }

    void set_int(const std::string &name, const int value) const {
        get_binder().set_int(name, value);
    }

    void set_uint(const std::string &name, const uint32_t value) const {
        get_binder().set_uint(name, value);
    }

    void set_float(const std::string &name, const float value) const {
        get_binder().set_float(name, value);
    }

    // Vectors
    void set_vec2(const std::string &name, const glm::vec2 &value) const {
        get_binder().set_vec2(name, value);
    }

    void set_vec2(const std::string &name, const float x, const float y) const {
        get_binder().set_vec2(name, glm::vec2(x, y));
    }

    void set_vec3(const std::string &name, const glm::vec3 &value) const {
        get_binder().set_vec3(name, value);
    }

    void set_vec3(const std::string &name, const float x, const float y, const float z) const {
        get_binder().set_vec3(name, glm::vec3(x, y, z));
    }

    void set_vec4(const std::string &name, const glm::vec4 &value) const {
        get_binder().set_vec4(name, value);
    }

    void set_vec4(const std::string &name, float x, float y, float z, float w) const {
        get_binder().set_vec4(name, glm::vec4(x, y, z, w));
    }

    // Matrices
    void set_mat2(const std::string &name, const glm::mat2 &value) const {
        get_binder().set_mat2(name, value);
    }

    void set_mat3(const std::string &name, const glm::mat3 &value) const {
        get_binder().set_mat3(name, value);
    }

    void set_mat4(const std::string &name, const glm::mat4 &value) const {
        get_binder().set_mat4(name, value);
    }

    // CONVENIENCE METHODS FOR COMMON GRAPHICS UNIFORMS 
    void set_transform_matrices(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) const {
        const auto binder = get_binder();
        binder.set_mat4("model", model);
        binder.set_mat4("view", view);
        binder.set_mat4("projection", projection);
        binder.set_mat4("MVP", projection * view * model);

        // Calculate and set normal matrix
        glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model)));
        binder.set_mat3("normalMatrix", normal_matrix);
    }

    void set_view_projection(const glm::mat4 &view, const glm::mat4 &projection) const {
        const auto binder = get_binder();
        binder.set_mat4("view", view);
        binder.set_mat4("projection", projection);
    }

    void set_camera_position(const glm::vec3 &position) const {
        get_binder().set_vec3("viewPos", position);
    }

    void set_time(const float time) const {
        get_binder().set_float("time", time);
    }

    void set_light_counts(const int directional_lights, const int point_lights) const {
        const auto binder = get_binder();
        binder.set_int("numDirectionalLights", directional_lights);
        binder.set_int("numPointLights", point_lights);
    }

    // Array setters for lights
    void set_directional_light(const int index, const glm::vec3 &direction, const glm::vec3 &color, const float intensity = 1.0f) const {
        std::string base = "directionalLights[" + std::to_string(index) + "]";
        const auto binder = get_binder();
        binder.set_vec3(base + ".direction", direction);
        binder.set_vec3(base + ".color", color);
        binder.set_float(base + ".intensity", intensity);
    }

    void set_point_light(const int index, const glm::vec3 &position, const glm::vec3 &color,
                         const float constant = 1.0f, const float linear = 0.09f, const float quadratic = 0.032f) const {
        std::string base = "pointLights[" + std::to_string(index) + "]";
        const auto binder = get_binder();
        binder.set_vec3(base + ".position", position);
        binder.set_vec3(base + ".color", color);
        binder.set_float(base + ".intensity", constant);
        binder.set_float(base + ".range", linear);
        binder.set_float(base + ".attenuation", quadratic);
    }

    // Texture binding
    void set_texture(const std::string &name, const int texture_unit) const {
        get_binder().set_int(name, texture_unit);
    }

    // Check if uniform exists
    bool has_uniform(const std::string &name) const {
        return get_binder().has_uniform(name);
    }

private:
    uint32_t program_id_;

    hellfire::ShaderUniformBinder get_binder() const {
        return hellfire::ShaderUniformBinder(program_id_);
    }
};
