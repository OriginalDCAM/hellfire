//
// Created by denzel on 13/08/2025.
//
#include "hellfire/graphics/managers/MaterialManager.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "hellfire/graphics/material/Material.h"

namespace hellfire {
    void MaterialManager::bind_material(const Material &material) {
        uint32_t shader_program = material.get_compiled_shader_id();
        if (shader_program == 0) {
            std::cerr << "Warning: Material " << material.get_name() << " has no compiled shader!" << std::endl;
            return;
        }

        material.bind();
    }

    void MaterialManager::bind_property_to_shader(const Material::Property &property, uint32_t shader_program,
                                                  int &texture_unit) {
        bind_property(property, shader_program, texture_unit);
    }

    void MaterialManager::bind_property(const Material::Property &property, uint32_t shader_program,
                                        int &texture_unit) {
        const std::string &uniform_name = property.uniform_name;

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
            case Material::PropertyType::COLOR3:
            case Material::PropertyType::VEC3: {
                glm::vec3 value = std::get<glm::vec3>(property.value);
                GLint location = glGetUniformLocation(shader_program, uniform_name.c_str());
                if (location != -1) {
                    glUniform3fv(location, 1, glm::value_ptr(value));
                }
                break;
            }

            case Material::PropertyType::COLOR4:
            case Material::PropertyType::VEC4: {
                glm::vec4 value = std::get<glm::vec4>(property.value);
                GLint location = glGetUniformLocation(shader_program, uniform_name.c_str());
                if (location != -1) {
                    glUniform4fv(location, 1, glm::value_ptr(value));
                }
                break;
            }

            case Material::PropertyType::TEXTURE: {
                Texture *texture = std::get<Texture *>(property.value);

                if (texture && texture->is_valid()) {
                    texture->bind(texture_unit);

                    GLint location = glGetUniformLocation(shader_program, uniform_name.c_str());
                    if (location != -1) {
                        glUniform1i(location, texture_unit);
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

    std::string MaterialManager::create_use_flag(const std::string &uniform_name) {
        return "use" + uniform_name;
    }
}
