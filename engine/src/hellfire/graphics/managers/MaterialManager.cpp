//
// Created by denzel on 13/08/2025.
//
#include "hellfire/graphics/managers/MaterialManager.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "hellfire/graphics/material/Material.h"
#include "hellfire/graphics/shader/Shader.h"

namespace hellfire {
    void MaterialManager::bind_material(const Material &material) {
        if (const uint32_t shader_program = material.get_compiled_shader_id(); shader_program == 0) {
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
        const ShaderUniformBinder binder(shader_program);
        const std::string &uniform_name = property.uniform_name;

        switch (property.type) {
            case Material::PropertyType::FLOAT: {
                const float value = std::get<float>(property.value);
                binder.set_float(uniform_name, value);
                break;
            }

            case Material::PropertyType::VEC2: {
                const glm::vec2 value = std::get<glm::vec2>(property.value);
                binder.set_vec2(uniform_name, value);
                break;
            }
            case Material::PropertyType::COLOR3:
            case Material::PropertyType::VEC3: {
                const glm::vec3 value = std::get<glm::vec3>(property.value);
                binder.set_vec3(uniform_name, value);
                break;
            }

            case Material::PropertyType::COLOR4:
            case Material::PropertyType::VEC4: {
                const glm::vec4 value = std::get<glm::vec4>(property.value);
                binder.set_vec4(uniform_name, value);
                break;
            }

            case Material::PropertyType::TEXTURE: {
                if (const Texture *texture = std::get<Texture *>(property.value); texture && texture->is_valid()) {
                    texture->bind(texture_unit);
                    binder.set_int(uniform_name, texture_unit);

                    // Set the texture usage flag if it exists
                    if (const char* flag_name = get_texture_flag_for_uniform(uniform_name)) {
                        binder.set_bool(flag_name, true);
                    }

                    texture_unit++;
                } else {
                    // Set texture usage flag to false for missing/invalid textures
                    if (const char* flag_name = get_texture_flag_for_uniform(uniform_name)) {
                        binder.set_bool(flag_name, false);
                    }

                    if (texture) {
                        std::cout << "Warning: Invalid texture for " << uniform_name << std::endl;
                    }
                }
                break;
            }

            case Material::PropertyType::BOOL: {
                const bool value = std::get<bool>(property.value);
                binder.set_bool(uniform_name, value);
                break;
            }

            case Material::PropertyType::INT: {
                const int value = std::get<int>(property.value);
                binder.set_int(uniform_name, value);
                break;
            }

            case Material::PropertyType::MAT3: {
                const glm::mat3 value = std::get<glm::mat3>(property.value);
                binder.set_mat3(uniform_name, value);
                break;
            }

            case Material::PropertyType::MAT4: {
                const glm::mat4 value = std::get<glm::mat4>(property.value);
                binder.set_mat4(uniform_name, value);
                break;
            }
            default: ;
        }
    }

    const char* MaterialManager::get_texture_flag_for_uniform(const std::string& uniform_name) {
        // Map texture uniform names to their corresponding flag names using constants
        if (uniform_name == MaterialConstants::DIFFUSE_TEXTURE) {
            return MaterialConstants::USE_DIFFUSE_TEXTURE;
        } else if (uniform_name == MaterialConstants::NORMAL_TEXTURE) {
            return MaterialConstants::USE_NORMAL_TEXTURE;
        } else if (uniform_name == MaterialConstants::SPECULAR_TEXTURE) {
            return MaterialConstants::USE_SPECULAR_TEXTURE;
        } else if (uniform_name == MaterialConstants::METALLIC_TEXTURE) {
            return MaterialConstants::USE_METALLIC_TEXTURE;
        } else if (uniform_name == MaterialConstants::ROUGHNESS_TEXTURE) {
            return MaterialConstants::USE_ROUGHNESS_TEXTURE;
        } else if (uniform_name == MaterialConstants::AO_TEXTURE) {
            return MaterialConstants::USE_AO_TEXTURE;
        } else if (uniform_name == MaterialConstants::EMISSIVE_TEXTURE) {
            return MaterialConstants::USE_EMISSIVE_TEXTURE;
        }
        
        // For custom textures, return nullptr (no automatic flag)
        return nullptr;
    }

}
