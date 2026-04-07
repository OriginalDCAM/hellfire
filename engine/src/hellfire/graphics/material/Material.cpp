//
// Created by denzel on 04/04/2025.
//
#include "hellfire/graphics/managers/ShaderManager.h"
#include "hellfire/graphics/material/Material.h"

#include "hellfire/core/Application.h"
#include "hellfire/utilities/ServiceLocator.h"

namespace hellfire {
    void Material::bind() const {
        uint32_t shader_program = get_compiled_shader_id();
        if (shader_program == 0) {
            std::cerr << "Warning: Material " << get_name() << " has no compiled shader!" << std::endl;
            return;
        }

        bound_texture_units_.clear();

        int texture_unit = 0;
        bind_all_properties(shader_program, texture_unit);
    }

    void Material::unbind() const {
        uint32_t shader_program = get_compiled_shader_id();
        if (shader_program == 0) return;

        unbind_all_textures();
    }

    void Material::unbind_all_textures() const {
        // Unbind all textures that were bound during the last bind() call
        for (int texture_unit : bound_texture_units_) {
            glActiveTexture(GL_TEXTURE0 + texture_unit);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        bound_texture_units_.clear();
    }

    void Material::bind_all_properties(const uint32_t shader_program, int &texture_unit) const {
        for (const auto &property: properties_ | std::views::values) {
            // Track texture unit usage
            int start_texture_unit = texture_unit;
            
            MaterialManager::bind_property_to_shader(property, shader_program, texture_unit);

            // If a texture was bound, track which unit it used
            if (property.type == PropertyType::TEXTURE && texture_unit > start_texture_unit) {
                bound_texture_units_.push_back(start_texture_unit);
            }
        }
    }

    std::shared_ptr<Material> MaterialBuilder::create(const std::string &name) {
        auto material = std::make_shared<Material>(name);

        // Set default colors
        material->set_diffuse_color(glm::vec3(0.8f));
        material->set_specular_color(glm::vec3(0.5f));

        // Set default material properties
        material->set_shininess(8.0f);
        material->set_opacity(1.0f);
        material->set_metallic(0.0f);
        material->set_roughness(0.5f);

        // Set default UV transforms
        material->set_uv_tiling(1.0f, 1.0f);
        material->set_uv_offset(glm::vec2(0.0f));
        material->set_uv_rotation(0.0f);

        // Initialize all texture usage flags to false
        material->set_property(MaterialConstants::USE_DIFFUSE_TEXTURE, false);
        material->set_property(MaterialConstants::USE_NORMAL_TEXTURE, false);
        material->set_property(MaterialConstants::USE_SPECULAR_TEXTURE, false);
        material->set_property(MaterialConstants::USE_METALLIC_TEXTURE, false);
        material->set_property(MaterialConstants::USE_ROUGHNESS_TEXTURE, false);
        material->set_property(MaterialConstants::USE_AO_TEXTURE, false);
        material->set_property(MaterialConstants::USE_EMISSIVE_TEXTURE, false);

        compile_shader_from_material(*material);

        return material;
    }

    std::shared_ptr<Material> MaterialBuilder::create_custom(const std::string &name, const std::string &vertex_path,
                                                             const std::string &fragment_path) {
        auto material = std::make_shared<Material>(name);
        material->set_custom_shader(vertex_path, fragment_path);

        compile_shader_from_material(*material);

        return material;
    }

    void MaterialBuilder::compile_shader_from_material(Material &material) {
        auto shader_id = ServiceLocator::get_service<ShaderManager>()->get_shader_for_material(material);
        material.set_compiled_shader_id(shader_id);
    }
}
