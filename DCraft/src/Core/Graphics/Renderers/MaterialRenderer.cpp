//
// Created by denzel on 13/08/2025.
//
#include "DCraft/Graphics/Materials/MaterialRenderer.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "DCraft/Graphics/Materials/Material.h"

namespace DCraft {
    void debug_shader_uniforms(uint32_t shader_program) {
        GLint num_uniforms = 0;
        glGetProgramiv(shader_program, GL_ACTIVE_UNIFORMS, &num_uniforms);
    
        std::cout << "=== Shader Uniforms (Total: " << num_uniforms << ") ===" << std::endl;
    
        for (GLint i = 0; i < num_uniforms; i++) {
            GLchar name[256];
            GLint size;
            GLenum type;
            glGetActiveUniform(shader_program, i, sizeof(name), nullptr, &size, &type, name);
        
            std::string type_name = (type == GL_SAMPLER_2D) ? "sampler2D" : 
                                   (type == GL_BOOL) ? "bool" : "other";
            std::cout << "  " << name << " (" << type_name << ")" << std::endl;
        }
        std::cout << "=============================================" << std::endl;
    }
    
    void MaterialRenderer::bind_material(const Material &material) {
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

    void MaterialRenderer::bind_property(const Material::Property &property, uint32_t shader_program,
        int &texture_unit) {
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

                if (texture && texture->is_valid()) {
                    texture->bind(texture_unit);

                    GLint location = glGetUniformLocation(shader_program, uniform_name.c_str());
                    if (location != -1) {
                        glUniform1i(location, texture_unit);
                        std::cout << "Bound texture to slot " << texture_unit 
                                  << " for uniform: " << uniform_name << std::endl; 
                    } else {
                        std::cout << "Warning: Uniform not found: " << uniform_name << std::endl;
                    }

                    std::string use_flag = create_use_flag(uniform_name);
                    GLint flag_location = glGetUniformLocation(shader_program, use_flag.c_str());
                    if (flag_location != -1) {
                        glUniform1i(flag_location, 1);
                    }

                    texture_unit++;
                } else {
                    // Handle invalid/missing texture
                    std::string use_flag = create_use_flag(uniform_name);
                    GLint flag_location = glGetUniformLocation(shader_program, use_flag.c_str());
                    if (flag_location != -1) {
                        glUniform1i(flag_location, 0);
                    }
        
                    if (texture) {
                        std::cout << "Warning: Invalid texture for " << uniform_name << std::endl;
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

    std::string MaterialRenderer::create_use_flag(const std::string &uniform_name) {
        // Convert "uDiffuseTexture" -> "useUDiffuseTexture"
        if (uniform_name.length() > 1 && uniform_name[0] == 'u' && std::isupper(uniform_name[1])) {
            char uniform_upper = std::toupper(uniform_name[1]);
            return "use" + uniform_name.substr(0, 1) + uniform_upper + uniform_name.substr(2);
        }
        return "use" + uniform_name;
    }

}
