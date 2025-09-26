#pragma once
#include <cstdint>
#include <string>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

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

    Shader(Shader &&other) noexcept : program_id_(other.program_id_), uniform_cache_(std::move(other.uniform_cache_)) {
        other.program_id_ = 0;
    }

    Shader &operator=(Shader &&other) noexcept {
        if (this != &other) {
            program_id_ = other.program_id_;
            uniform_cache_ = std::move(other.uniform_cache_);
            other.program_id_ = 0;
        }
        return *this;
    }

    static Shader from_id(uint32_t shader_id) {
        return Shader{shader_id};
    }

    // Usage
    void use() const {
        if (program_id_ != 0) {
            glUseProgram(program_id_);
        }
    }

    uint32_t get_program_id() const { return program_id_; }
    bool is_valid() const { return program_id_ != 0; }

    // === UNIFORM SETTERS ===
    void set_bool(const std::string &name, bool value) {
        glUniform1i(get_uniform_location(name), static_cast<int>(value));
    }

    void set_int(const std::string &name, int value) {
        glUniform1i(get_uniform_location(name), value);
    }

    void set_float(const std::string &name, float value) {
        glUniform1f(get_uniform_location(name), value);
    }

    // Vectors
    void set_vec2(const std::string &name, const glm::vec2 &value) {
        glUniform2fv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    void set_vec2(const std::string &name, float x, float y) {
        glUniform2f(get_uniform_location(name), x, y);
    }

    void set_vec3(const std::string &name, const glm::vec3 &value) {
        glUniform3fv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    void set_vec3(const std::string &name, float x, float y, float z) {
        glUniform3f(get_uniform_location(name), x, y, z);
    }

    void set_vec4(const std::string &name, const glm::vec4 &value) {
        glUniform4fv(get_uniform_location(name), 1, glm::value_ptr(value));
    }

    void set_vec4(const std::string &name, float x, float y, float z, float w) {
        glUniform4f(get_uniform_location(name), x, y, z, w);
    }

    // Matrices
    void set_mat2(const std::string &name, const glm::mat2 &value) {
        glUniformMatrix2fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void set_mat3(const std::string &name, const glm::mat3 &value) {
        glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void set_mat4(const std::string &name, const glm::mat4 &value) {
        glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    // === CONVENIENCE METHODS FOR COMMON GRAPHICS UNIFORMS ===

    void set_transform_matrices(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) {
        set_mat4("model", model);
        set_mat4("view", view);
        set_mat4("projection", projection);
        set_mat4("MVP", projection * view * model);

        // Calculate and set normal matrix
        glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model)));
        set_mat3("normalMatrix", normal_matrix);
    }

    void set_view_projection(const glm::mat4 &view, const glm::mat4 &projection) {
        set_mat4("view", view);
        set_mat4("projection", projection);
    }

    void set_camera_position(const glm::vec3 &position) {
        set_vec3("viewPos", position);
    }

    void set_time(float time) {
        set_float("time", time);
    }

    void set_light_counts(int directional_lights, int point_lights) {
        set_int("numDirectionalLights", directional_lights);
        set_int("numPointLights", point_lights);
    }

    // Array setters for lights
    void set_directional_light(int index, const glm::vec3 &direction, const glm::vec3 &color, float intensity = 1.0f) {
        std::string base = "directionalLights[" + std::to_string(index) + "]";
        set_vec3(base + ".direction", direction);
        set_vec3(base + ".color", color);
        set_float(base + ".intensity", intensity);
    }

    void set_point_light(int index, const glm::vec3 &position, const glm::vec3 &color,
                         float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f) {
        std::string base = "pointLights[" + std::to_string(index) + "]";
        set_vec3(base + ".position", position);
        set_vec3(base + ".color", color);
        set_float(base + ".intensity", constant);
        set_float(base + ".range", linear);
        set_float(base + ".attenuation", quadratic);
    }

    // Texture binding
    void set_texture(const std::string &name, int texture_unit) {
        set_int(name, texture_unit);
    }

    // Check if uniform exists
    bool has_uniform(const std::string &name) {
        return get_uniform_location(name) != -1;
    }

private:
    uint32_t program_id_;
    mutable std::unordered_map<std::string, GLint> uniform_cache_;

    GLint get_uniform_location(const std::string &name) const {
        if (program_id_ == 0) return -1;

        // Check cache first
        auto it = uniform_cache_.find(name);
        if (it != uniform_cache_.end()) {
            return it->second;
        }

        // Get location and cache it
        GLint location = glGetUniformLocation(program_id_, name.c_str());
        uniform_cache_[name] = location;
        return location;
    }
};
