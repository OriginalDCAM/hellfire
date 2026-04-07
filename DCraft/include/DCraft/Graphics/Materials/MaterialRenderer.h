//
// MaterialRenderer.h - Material binding for your Material class
//
#pragma once
#include <GL/glew.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "Material.h"

namespace DCraft {
    class MaterialRenderer {
    public:
        static void bind_material(const Material& material) {
            uint32_t shader_program = material.get_compiled_shader_id();
            if (shader_program == 0) {
                std::cerr << "Warning: Material " << material.get_name() << " has no compiled shader!" << std::endl;
                return;
            }
            
            int texture_unit = 0;
            
            for (const auto& [name, property] : material.get_properties()) {
                bind_property(property, shader_program, texture_unit);
            }
        }
        
    private:
        static void bind_property(const Material::Property& property, uint32_t shader_program, int& texture_unit) {
            // Use custom uniform name if specified, otherwise use property name
            const std::string& uniform_name = property.uniform_name;
            
            switch (property.type) {
                case Material::PropertyType::FLOAT: {
                    float value = std::get<float>(property.value);
                    GLint location = glGetUniformLocation(shader_program, uniform_name.c_str());
                    if (location != -1) {
                        glUniform1f(location, value);
                    }
                    break;
                }
                
                case Material::PropertyType::VEC2: {
                    glm::vec2 value = std::get<glm::vec2>(property.value);
                    GLint location = glGetUniformLocation(shader_program, uniform_name.c_str());
                    if (location != -1) {
                        glUniform2fv(location, 1, glm::value_ptr(value));
                    }
                    break;
                }
                
                case Material::PropertyType::VEC3: {
                    glm::vec3 value = std::get<glm::vec3>(property.value);
                    GLint location = glGetUniformLocation(shader_program, uniform_name.c_str());
                    if (location != -1) {
                        glUniform3fv(location, 1, glm::value_ptr(value));
                    }
                    break;
                }
                
                case Material::PropertyType::VEC4: {
                    glm::vec4 value = std::get<glm::vec4>(property.value);
                    GLint location = glGetUniformLocation(shader_program, uniform_name.c_str());
                    if (location != -1) {
                        glUniform4fv(location, 1, glm::value_ptr(value));
                    }
                    break;
                }
                
                case Material::PropertyType::TEXTURE: {
                    Texture* texture = std::get<Texture*>(property.value);
                    if (texture) {
                        glActiveTexture(GL_TEXTURE0 + texture_unit);
                        texture->bind();
                        
                        GLint location = glGetUniformLocation(shader_program, uniform_name.c_str());
                        if (location != -1) {
                            glUniform1i(location, texture_unit);
                        }
                        
                        // Also set usage flag (for backwards compatibility with your existing shaders)
                        std::string use_flag = "use" + capitalize_first(uniform_name);
                        GLint flag_location = glGetUniformLocation(shader_program, use_flag.c_str());
                        if (flag_location != -1) {
                            glUniform1i(flag_location, 1);
                        }
                        
                        texture_unit++;
                    } else {
                        // Set usage flag to false if no texture
                        std::string use_flag = "use" + capitalize_first(uniform_name);
                        GLint flag_location = glGetUniformLocation(shader_program, use_flag.c_str());
                        if (flag_location != -1) {
                            glUniform1i(flag_location, 0);
                        }
                    }
                    break;
                }
                
                case Material::PropertyType::BOOL: {
                    bool value = std::get<bool>(property.value);
                    GLint location = glGetUniformLocation(shader_program, uniform_name.c_str());
                    if (location != -1) {
                        glUniform1i(location, value ? 1 : 0);
                    }
                    break;
                }
                
                case Material::PropertyType::INT: {
                    int value = std::get<int>(property.value);
                    GLint location = glGetUniformLocation(shader_program, uniform_name.c_str());
                    if (location != -1) {
                        glUniform1i(location, value);
                    }
                    break;
                }
                
                case Material::PropertyType::MAT3: {
                    glm::mat3 value = std::get<glm::mat3>(property.value);
                    GLint location = glGetUniformLocation(shader_program, uniform_name.c_str());
                    if (location != -1) {
                        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
                    }
                    break;
                }
                
                case Material::PropertyType::MAT4: {
                    glm::mat4 value = std::get<glm::mat4>(property.value);
                    GLint location = glGetUniformLocation(shader_program, uniform_name.c_str());
                    if (location != -1) {
                        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
                    }
                    break;
                }
            }
        }
        
        static std::string capitalize_first(const std::string& str) {
            if (str.empty()) return str;
            std::string result = str;
            result[0] = std::toupper(result[0]);
            return result;
        }
    };
}