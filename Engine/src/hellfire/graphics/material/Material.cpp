//
// Created by denzel on 04/04/2025.
//
#include "hellfire/graphics/managers/ShaderManager.h"
#include "hellfire/graphics/material/Material.h"

#include "hellfire/core/Application.h"
#include "hellfire/utilities/ServiceLocator.h"

namespace hellfire {
    bool Material::is_transparent() const {
        return get_property<float>("uOpacity", 1.0f) < 1.0f;
    }

    void Material::set_texture(const std::string &path, TextureType type) {
        auto* texture = new Texture(path, type);
        if (type == TextureType::DIFFUSE) {
            set_diffuse_texture(texture);
        }
    }
    
    void Material::set_texture(const std::shared_ptr<Texture> &texture) {
        if (texture->get_type() == TextureType::DIFFUSE) {
            set_diffuse_texture(texture.get());
        }
    }

    Material & Material::add_texture(const std::string &path, const std::string &uniform_name, int texture_slot) {
        auto* texture = new Texture(path);
        
        if (texture_slot >= 0) {
            // Store the texture slot
            texture->set_slot(texture_slot);
            
            set_property(uniform_name, texture, uniform_name);
            set_property(uniform_name + "Slot", texture_slot);
        } else {
            set_property(uniform_name, texture, uniform_name);
        }

        set_texture_usage_flag(uniform_name, true);
        
        return *this;
    }

    
    Material & Material::add_texture(Texture &texture, const std::string &uniform_name, int texture_slot) {
        
        if (texture_slot >= 0) {
            // Store the texture slot
            texture.set_slot(texture_slot);
            
            set_property(uniform_name, &texture, uniform_name);
            set_property(uniform_name + "Slot", texture_slot);
        } else {
            set_property(uniform_name, &texture, uniform_name);
        }

        set_texture_usage_flag(uniform_name, true);
        
        return *this;
    }

    void Material::bind() const {
        uint32_t shader_program = get_compiled_shader_id();
        if (shader_program  == 0) {
            std::cerr << "Warning: Material " << get_name() << " has no compiled shader!" << std::endl;
            return;
        }

        int texture_unit = 0;

            bind_all_properties(shader_program, texture_unit);
    }

    void Material::unbind() const {
        uint32_t shader_program = get_compiled_shader_id();
        if (shader_program == 0) return;

        int texture_unit = 0;

        for (const std::string& uniform_name : touched_uniforms_) {
            if (base_material_->has_property(uniform_name)) {
                auto base_property = base_material_->get_property_object(uniform_name);
                MaterialManager::bind_property_to_shader(base_property, shader_program, texture_unit);
            }
        }
        touched_uniforms_.clear();
    }

    void Material::set_texture_usage_flag(const std::string &uniform_name, bool value) {
        if (uniform_name == "uDiffuseTexture") {
            set_property("useUDiffuseTexture", value);
        } else if (uniform_name == "uNormalTexture") {
            set_property("useUNormalTexture", value);
        } else if (uniform_name == "uSpecularTexture") {
            set_property("useUSpecularTexture", value);
        }
    }

    void Material::bind_all_properties(const uint32_t shader_program, int& texture_unit) const {
        for (const auto &property: properties_ | std::views::values) {
            MaterialManager::bind_property_to_shader(property, shader_program, texture_unit);
        }
    }

    std::shared_ptr<Material> MaterialBuilder::create(const std::string &name) {
        auto material = std::make_shared<Material>(name);
        
        material->set_property("uDiffuseColor", glm::vec3(0.8f), Material::PropertyType::COLOR3);
        material->set_property("uSpecularColor", glm::vec3(0.5f), Material::PropertyType::COLOR3);

        material->set_shininess(32.0f);
        material->set_uv_tiling(1.0f, 1.0f);  
        material->set_uv_offset(glm::vec2(0.0f, 0.0f)); 
        material->set_uv_rotation(0.0f);
        
        material->set_property("uOpacity", 1.0f);

        material->set_property("uvTiling", glm::vec2(1.0f));
        material->set_property("uvOffset", glm::vec2(0.0f));
        material->set_property("uvRotation", 0.0f);

        
        material->set_property("useUDiffuseTexture", false);
        material->set_property("useUNormalTexture", false);
        material->set_property("useUSpecularTexture", false);

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
