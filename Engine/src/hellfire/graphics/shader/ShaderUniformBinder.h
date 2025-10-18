//
// Created by denzel on 15/10/2025.
//

#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

#include "GL/glew.h"
#include "glm/detail/type_vec.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace hellfire {
    /**
     * Abstraction layer for shader uniform binding
     * Hides OpenGL implementation details and provides uniform caching
     */
    class ShaderUniformBinder {
    public:
        explicit ShaderUniformBinder(const uint32_t shader_program) 
            : shader_program_(shader_program) {}

        // Scalar Types 
        void set_bool(const std::string& name, bool value) const {
            if (const GLint location = get_uniform_location(name); location != -1) {
                glUniform1i(location, value ? 1 : 0);
            }
        }

        void set_int(const std::string& name, int value) const {
            if (const GLint location = get_uniform_location(name); location != -1) {
                glUniform1i(location, value);
            }
        }

        void set_float(const std::string& name, float value) const {
            if (const GLint location = get_uniform_location(name); location != -1) {
                glUniform1f(location, value);
            }
        }

        // Vector Types 
        void set_vec2(const std::string& name, const glm::vec2& value) const {
            if (const GLint location = get_uniform_location(name); location != -1) {
                glUniform2fv(location, 1, glm::value_ptr(value));
            }
        }

        void set_vec3(const std::string& name, const glm::vec3& value) const {
            if (const GLint location = get_uniform_location(name); location != -1) {
                glUniform3fv(location, 1, glm::value_ptr(value));
            }
        }

        void set_vec4(const std::string& name, const glm::vec4& value) const {
            if (const GLint location = get_uniform_location(name); location != -1) {
                glUniform4fv(location, 1, glm::value_ptr(value));
            }
        }

        // Matrix Types
        void set_mat2(const std::string& name, const glm::mat2& value) const {
            if (const GLint location = get_uniform_location(name); location != -1) {
                glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
            }
        }
        
        void set_mat3(const std::string& name, const glm::mat3& value) const {
            if (const GLint location = get_uniform_location(name); location != -1) {
                glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
            }
        }

        void set_mat4(const std::string& name, const glm::mat4& value) const {
            if (const GLint location = get_uniform_location(name); location != -1) {
                glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
            }
        }

        // Utility Methods 
        bool has_uniform(const std::string& name) const {
            return get_uniform_location(name) != -1;
        }

        void clear_cache() {
            uniform_cache_.clear();
        }

        uint32_t get_shader_program() const {
            return shader_program_;
        }

    private:
        uint32_t shader_program_;
        mutable std::unordered_map<std::string, GLint> uniform_cache_;

        GLint get_uniform_location(const std::string& name) const {
            if (shader_program_ == 0) return -1;

            // Check cache first
            if (const auto it = uniform_cache_.find(name); it != uniform_cache_.end()) {
                return it->second;
            }

            // Get location and cache it
            const GLint location = glGetUniformLocation(shader_program_, name.c_str());
            uniform_cache_[name] = location;
            
            return location;
        }
    };
}
