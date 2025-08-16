//
// Created by denzel on 04/04/2025.
//
#include "DCraft/Graphics/Materials/Material.h"

#include "DCraft/Application.h"

namespace DCraft {
    bool Material::is_transparent() const {
        float alpha = get_property<float>("alpha", 1.0f);
        float transparency = get_property<float>("transparency", 1.0f);
        bool use_transparency = get_property<bool>("useTransparency", false);
        
        return (alpha < 1.0f) || (transparency < 1.0f) || use_transparency;
    }

    void Material::set_texture(const std::string &path, TextureType type) {
        auto* texture = new Texture(path, type);
        if (type == TextureType::DIFFUSE) {
            set_diffuse_texture(texture);
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
        
        return *this;
    }

    std::unique_ptr<Material> MaterialBuilder::create_lambert(const std::string &name) {
        auto material = std::make_unique<Material>(name);
        material->set_builtin_material_type(0);
        material->set_diffuse_color(glm::vec3(0.8f));
        material->set_ambient_color(glm::vec3(0.1f));
        material->set_uv_tiling(1.0f, 1.0f);  
        material->set_uv_offset(glm::vec2(0.0f, 0.0f)); 
        material->set_uv_rotation(0.0f);

        compile_shader_from_material(*material);

        return material;
    }

    std::unique_ptr<Material> MaterialBuilder::create_phong(const std::string &name) {
        auto material = std::make_unique<Material>(name);
        material->set_builtin_material_type(1);
        material->set_diffuse_color(glm::vec3(0.8f));
        material->set_ambient_color(glm::vec3(0.3f));
        material->set_specular_color(glm::vec3(0.5f));
        material->set_shininess(32.0f);
        material->set_uv_tiling(1.0f, 1.0f);  
        material->set_uv_offset(glm::vec2(0.0f, 0.0f)); 
        material->set_uv_rotation(0.0f);

        compile_shader_from_material(*material);
        
        return material;
    }

    std::unique_ptr<Material> MaterialBuilder::create_pbr(const std::string &name) {
        auto material = std::make_unique<Material>(name);
        material->set_builtin_material_type(2);
        material->set_diffuse_color(glm::vec3(0.8f));
        material->set_property("metallic", 0.0f);
        material->set_property("roughness", 0.5f);
        material->set_property("ao", 1.0f);
        material->set_uv_tiling(1.0f, 1.0f);  
        material->set_uv_offset(glm::vec2(0.0f, 0.0f)); 
        material->set_uv_rotation(0.0f);

        compile_shader_from_material(*material);

        return material;
    }

    std::unique_ptr<Material> MaterialBuilder::create_custom(const std::string &name, const std::string &vertex_path,
        const std::string &fragment_path) {
        auto material = std::make_unique<Material>(name);
        material->set_custom_shader(vertex_path, fragment_path);
        
        compile_shader_from_material(*material);
        
        return material;
    }

    std::unique_ptr<Material> MaterialBuilder::create_from_template(const std::string &name,
        const std::string &template_name) {
        auto material = std::make_unique<Material>(name);
            
        // Built-in templates
        if (template_name == "toon") {
            material->set_custom_shader("assets/Shaders/toon.vert", "assets/Shaders/toon.frag");
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

    std::unique_ptr<Material> MaterialChain::build() {
        uint32_t shader_id = shader_manager_.get_shader_for_material(*material_);
        material_->set_compiled_shader_id(shader_id);
        return std::move(material_);
    }

    MaterialChain lambert(const std::string &name, DCraft::ShaderManager &shader_manager) {
        auto material = std::make_unique<Material>(name);
        material->set_builtin_material_type(0);
        material->set_diffuse_color(glm::vec3(0.8f));
        material->set_ambient_color(glm::vec3(0.1f));
        return MaterialChain(std::move(material), shader_manager);
    }

    MaterialChain phong(const std::string &name, DCraft::ShaderManager &shader_manager) {
        auto material = std::make_unique<Material>(name);
        material->set_builtin_material_type(1);
        material->set_diffuse_color(glm::vec3(0.8f));
        material->set_ambient_color(glm::vec3(0.3f));
        material->set_specular_color(glm::vec3(0.5f));
        material->set_shininess(32.0f);
        return MaterialChain(std::move(material), shader_manager);
    }

    MaterialChain custom(const std::string &name, const std::string &vertex_path, const std::string &fragment_path,
        DCraft::ShaderManager &shader_manager) {
        auto material = std::make_unique<Material>(name);
        material->set_custom_shader(vertex_path, fragment_path);
        return MaterialChain(std::move(material), shader_manager);
    }
}
