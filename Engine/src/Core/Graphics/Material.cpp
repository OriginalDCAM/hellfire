//
// Created by denzel on 04/04/2025.
//
#include "DCraft/Graphics/Managers/ShaderManager.h"
#include "DCraft/Graphics/Materials/Material.h"

#include "DCraft/Application.h"

namespace DCraft {
    bool Material::is_transparent() const {
        float alpha = get_property<float>("uAlpha", 1.0f);
        float transparency = get_property<float>("uTransparency", 1.0f);
        bool use_transparency = get_property<bool>("useTransparency", false);
        
        return (alpha < 1.0f) || (transparency < 1.0f) || use_transparency;
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

        if (is_instance()) {
            bind_base_properties(shader_program, texture_unit);

            for (const auto& [name, property] : overrides_) {
                MaterialRenderer::bind_property_to_shader(property, shader_program, texture_unit);
                touched_uniforms_.insert(name);
            }
        } else {
            bind_all_properties(shader_program, texture_unit);
        }
    }

    void Material::unbind() const {
        if (!is_instance()) return; // Only instances need unbinding

        uint32_t shader_program = get_compiled_shader_id();
        if (shader_program == 0) return;

        int texture_unit = 0;

        for (const std::string& uniform_name : touched_uniforms_) {
            if (base_material_->has_property(uniform_name)) {
                auto base_property = base_material_->get_property_object(uniform_name);
                MaterialRenderer::bind_property_to_shader(base_property, shader_program, texture_unit);
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
        for (const auto& [name, property] : properties_) {
            MaterialRenderer::bind_property_to_shader(property, shader_program, texture_unit);
        }
    }

    std::shared_ptr<Material> MaterialBuilder::create_lambert(const std::string &name) {
        auto material = std::make_shared<Material>(name);
        material->set_builtin_material_type(0);
        material->set_diffuse_color(glm::vec3(0.8f));
        material->set_ambient_color(glm::vec3(0.1f));
        material->set_uv_tiling(1.0f, 1.0f);  
        material->set_uv_offset(glm::vec2(0.0f, 0.0f)); 
        material->set_uv_rotation(0.0f);

        material->set_property("uDiffuseColor", glm::vec3(0.8f, 0.8f, 0.8f));
        material->set_property("uAmbientColor", glm::vec3(0.1f, 0.1f, 0.1f));
        material->set_property("uSpecularColor", glm::vec3(0.0f, 0.0f, 0.0f));
    
        material->set_property("uAlpha", 1.0f);
        material->set_property("uTransparency", 1.0f);
        material->set_property("uOpacity", 1.0f);
        material->set_property("useTransparency", false);
    
        // UV defaults
        material->set_property("uvTiling", glm::vec2(1.0f));
        material->set_property("uvOffset", glm::vec2(0.0f));
        material->set_property("uvRotation", 0.0f);
    
        // Texture usage flags - all false initially
        material->set_property("useUDiffuseTexture", false);
        material->set_property("useUNormalTexture", false);
        material->set_property("useUSpecularTexture", false);

        compile_shader_from_material(*material);

        return material;
    }

    std::shared_ptr<Material> MaterialBuilder::create_phong(const std::string &name) {
        auto material = std::make_shared<Material>(name);
        material->set_builtin_material_type(1);
        material->set_diffuse_color(glm::vec3(0.8f));
        material->set_ambient_color(glm::vec3(0.3f));
        material->set_specular_color(glm::vec3(0.5f));
        material->set_shininess(32.0f);
        material->set_uv_tiling(1.0f, 1.0f);  
        material->set_uv_offset(glm::vec2(0.0f, 0.0f)); 
        material->set_uv_rotation(0.0f);
        
        material->set_property("uAlpha", 1.0f);
        material->set_property("uTransparency", 1.0f);
        material->set_property("uOpacity", 1.0f);
        material->set_property("useTransparency", false);

        material->set_property("uvTiling", glm::vec2(1.0f));
        material->set_property("uvOffset", glm::vec2(0.0f));
        material->set_property("uvRotation", 0.0f);

        
        material->set_property("useUDiffuseTexture", false);
        material->set_property("useUNormalTexture", false);
        material->set_property("useUSpecularTexture", false);

        compile_shader_from_material(*material);
        
        return material;
    }

    std::shared_ptr<Material> MaterialBuilder::create_pbr(const std::string &name) {
        auto material = std::make_shared<Material>(name);
        material->set_builtin_material_type(2);
        material->set_diffuse_color(glm::vec3(0.8f));
        material->set_property("uMetallic", 0.0f);
        material->set_property("uRoughness", 0.5f);
        material->set_property("uAO", 1.0f);
        material->set_uv_tiling(1.0f, 1.0f);  
        material->set_uv_offset(glm::vec2(0.0f, 0.0f)); 
        material->set_uv_rotation(0.0f);

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

    std::shared_ptr<Material> MaterialBuilder::create_from_template(const std::string &name,
                                                                    const std::string &template_name) {
        auto material = std::make_shared<Material>(name);
            
        // Built-in templates
        if (template_name == "toon") {
            material->set_custom_shader("assets/shaders/toon.vert", "assets/shaders/toon.frag");
            material->set_property("toonSteps", 4.0f);
            material->set_property("outlineWidth", 0.02f);
        }

        compile_shader_from_material(*material);
            
        return material;
    }

    void MaterialBuilder::compile_shader_from_material(Material &material) {
        auto shader_id = Application::get_instance().get_shader_manager().get_shader_for_material(material);
        material.set_compiled_shader_id(shader_id);
    }

    std::shared_ptr<Material> MaterialChain::build() {
        uint32_t shader_id = shader_manager_.get_shader_for_material(*material_);
        material_->set_compiled_shader_id(shader_id);
        return material_;
    }

    MaterialChain lambert(const std::string &name, ShaderManager &shader_manager) {
        auto material = std::make_shared<Material>(name);
        material->set_builtin_material_type(0);
        material->set_diffuse_color(glm::vec3(0.8f));
        material->set_ambient_color(glm::vec3(0.1f));
        return MaterialChain{material, shader_manager};
    }

    MaterialChain phong(const std::string &name, ShaderManager &shader_manager) {
        auto material = std::make_shared<Material>(name);
        material->set_builtin_material_type(1);
        material->set_diffuse_color(glm::vec3(0.8f));
        material->set_ambient_color(glm::vec3(0.3f));
        material->set_specular_color(glm::vec3(0.5f));
        material->set_shininess(32.0f);
        return MaterialChain{material, shader_manager};
    }

    MaterialChain custom(const std::string &name, const std::string &vertex_path, const std::string &fragment_path,
        ShaderManager &shader_manager) {
        auto material = std::make_shared<Material>(name);
        material->set_custom_shader(vertex_path, fragment_path);
        return MaterialChain{material, shader_manager};
    }
}
