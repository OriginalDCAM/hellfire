#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <optional>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include "DCraft/Graphics/Textures/Texture.h"

namespace DCraft {
    class Material {
    public:
        enum class PropertyType {
            FLOAT, VEC2, VEC3, VEC4, TEXTURE, BOOL, INT, MAT3, MAT4
        };

        struct Property {
            PropertyType type;
            std::string name;
            std::variant<float, glm::vec2, glm::vec3, glm::vec4, Texture*, bool, int, glm::mat3, glm::mat4> value;

            std::string uniform_name;

            Property() {}
            Property(const std::string& name, float value, const std::string& uniform = "") : type(PropertyType::FLOAT), name(name), value(value), uniform_name(uniform.empty() ? name : uniform) {}
            Property(const std::string& name, const glm::vec2& value, const std::string& uniform = "") : type(PropertyType::VEC2), value(value), uniform_name(uniform.empty() ? name : uniform), name(name) {}
            Property(const std::string& name, const glm::vec3& value, const std::string& uniform = "") : type(PropertyType::VEC3), value(value), uniform_name(uniform.empty() ? name : uniform), name(name) {}
            Property(const std::string& name, const glm::vec4& value, const std::string& uniform = "") : name(name), type(PropertyType::VEC4), value(value), uniform_name(uniform.empty() ? name : uniform){}
            Property(const std::string& name, Texture* value, const std::string& uniform = "") : name(name), type(PropertyType::TEXTURE), value(value), uniform_name(uniform.empty() ? name : uniform){}
            Property(const std::string& name, bool value, const std::string& uniform = "") : name(name), type(PropertyType::BOOL), value(value), uniform_name(uniform.empty() ? name : uniform){}
            Property(const std::string& name, int value, const std::string& uniform = "") : name(name), type(PropertyType::INT), value(value), uniform_name(uniform.empty() ? name : uniform){}
            Property(const std::string& name, const glm::mat3& value, const std::string& uniform = "") : name(name), type(PropertyType::MAT3), value(value), uniform_name(uniform.empty() ? name : uniform){}
            Property(const std::string& name, const glm::mat4& value, const std::string& uniform = "") : name(name), type(PropertyType::MAT4), value(value), uniform_name(uniform.empty() ? name : uniform){}
        };

        struct ShaderInfo {
            std::string vertex_path;
            std::string fragment_path;
            std::optional<std::string> geometry_path;  // Optional geometry shader
            std::unordered_set<std::string> defines;   // Preprocessor defines
            std::unordered_map<std::string, std::string> uniform_mappings; // Property name -> uniform name
            
            bool is_valid() const {
                return !vertex_path.empty() && !fragment_path.empty();
            }
        };

    private:
        std::string name_;
        std::unordered_map<std::string, Property> properties_;

        // ===Shader information===
        std::optional<ShaderInfo> custom_shader_info_;
        uint32_t compiled_shader_id_ = 0;
        
        // ===Built-in material type (fallback if no custom shader)===
        int builtin_material_type_ = 0; // 0=Lambert, 1=Phong, 2=PBR

    public:
        Material(const std::string& name) : name_(name) {}

        // ===Generic property setters===
        void set_property(const std::string& name, const float value, const std::string& uniform_name = "") {
            properties_[name] = Property(name, value, uniform_name);
        }

        void set_property(const std::string& name, const glm::vec2& value, const std::string& uniform_name = "") {
            properties_[name] = Property(name, value, uniform_name);
        }

        void set_property(const std::string& name, const glm::vec3& value, const std::string& uniform_name = "") {
            properties_[name] = Property(name, value, uniform_name);
        }

        void set_property(const std::string& name, const glm::vec4& value, const std::string& uniform_name = "") {
            properties_[name] = Property(name, value, uniform_name);
        }
        
        void set_property(const std::string& name, Texture* value, const std::string& uniform_name = "") {
            properties_[name] = Property(name, value, uniform_name);
        }

        void set_property(const std::string& name, const bool value, const std::string& uniform_name = "") {
            properties_[name] = Property(name, value, uniform_name);
        }

        void set_property(const std::string& name, const int value, const std::string& uniform_name = "") {
            properties_[name] = Property(name, value, uniform_name);
        }

        void set_property(const std::string& name, const glm::mat3& value, const std::string& uniform_name = "") {
            properties_[name] = Property(name, value, uniform_name);
        }

        void set_property(const std::string& name, const glm::mat4& value, const std::string& uniform_name = "") {
            properties_[name] = Property(name, value, uniform_name);
        }

        // ===Custom shader support===
        void set_custom_shader(const std::string& vertex_path, const std::string& fragment_path) {
            custom_shader_info_ = ShaderInfo{vertex_path, fragment_path};
        }
        
        void set_custom_shader(const ShaderInfo& shader_info) {
            custom_shader_info_ = shader_info;
        }
        
        void add_shader_define(const std::string& define) {
            if (custom_shader_info_) {
                custom_shader_info_->defines.insert(define);
            }
        }
        
        void set_uniform_mapping(const std::string& property_name, const std::string& uniform_name) {
            if (custom_shader_info_) {
                custom_shader_info_->uniform_mappings[property_name] = uniform_name;
            }
        }
        
        bool has_custom_shader() const {
            return custom_shader_info_ && custom_shader_info_->is_valid();
        }
        
        const ShaderInfo* get_shader_info() const {
            return custom_shader_info_ ? &*custom_shader_info_ : nullptr;
        }
        
        void set_compiled_shader_id(uint32_t shader_id) {
            compiled_shader_id_ = shader_id;
        }
        
        uint32_t get_compiled_shader_id() const {
            return compiled_shader_id_;
        }
        
        // === BUILT-IN MATERIAL TYPE ===
        void set_builtin_material_type(int type) {
            builtin_material_type_ = type;
        }
        
        int get_builtin_material_type() const {
            return builtin_material_type_;
        }
        
        // ===Convenience methods===
        void set_diffuse_color(const glm::vec3& color) { set_property("diffuseColor", color); }
        void set_ambient_color(const glm::vec3& color) { set_property("ambientColor", color); }
        void set_specular_color(const glm::vec3& color) { set_property("specularColor", color); }
        void set_shininess(const float shininess) { set_property("shininess", shininess); }
        void set_diffuse_texture(const Texture* texture) { set_property("diffuseTexture", texture); }

        void set_uv_tiling(const glm::vec2& tiling) { 
            set_property("uvTiling", tiling); 
        }
    
        void set_uv_tiling(float x, float y) { 
            set_uv_tiling(glm::vec2(x, y)); 
        }
    
        void set_uv_offset(const glm::vec2& offset) { 
            set_property("uvOffset", offset); 
        }
    
        void set_uv_rotation(float rotation) { 
            set_property("uvRotation", rotation); 
        }

        //== Transparency convenience methods===
        void set_transparency(float alpha) {
            set_property("alpha", alpha);
            set_property("useTransparency", alpha < 1.0f);
        }
    
        void set_transparent(bool transparent) {
            set_property("useTransparency", transparent);
        }
    
        bool is_transparent() const {
            float alpha = get_property<float>("alpha", 1.0f);
            float transparency = get_property<float>("transparency", 1.0f);
            bool use_transparency = get_property<bool>("useTransparency", false);
        
            return (alpha < 1.0f) || (transparency < 1.0f) || use_transparency;
        }
    
        // Quick presets for common tiling patterns
        void set_brick_tiling() { set_uv_tiling(4.0f, 2.0f); }
        void set_tile_pattern() { set_uv_tiling(8.0f, 8.0f); }
        void set_wood_planks() { set_uv_tiling(1.0f, 4.0f); }

        void set_diffuse_texture(Texture* texture) { 
            set_property("diffuseTexture", texture); 
            set_property("useDiffuseTexture", texture != nullptr);
        }
        
        void set_texture(const std::string& path, TextureType type) {
            auto* texture = new Texture(path, type);
            if (type == TextureType::DIFFUSE) {
                set_diffuse_texture(texture);
            }
        }

        // === Getters ===
        template<typename T>
        T get_property(const std::string& name, const T& default_value = T{}) const {
            auto it = properties_.find(name);
            if (it != properties_.end()) {
                if (auto* val = std::get_if<T>(&it->second.value)) {
                    return *val;
                }
            }
            return default_value;
        }

        const auto& get_properties() const { return properties_; }

        void set_name(const std::string& name) {
            name_ = name;
        } 
        const std::string& get_name() const { return name_; }

        // Backwards compatibility
        glm::vec3 get_ambient_color() const { return get_property<glm::vec3>("ambientColor", glm::vec3(0.1f)); }
        glm::vec3 get_diffuse_color() const { return get_property<glm::vec3>("diffuseColor", glm::vec3(0.8f)); }
        glm::vec3 get_specular_color() const { return get_property<glm::vec3>("specularColor", glm::vec3(0.5f)); }
        float get_shininess() const { return get_property<float>("shininess", 32.0f); }
    };


    class MaterialBuilder {
            public:
        static std::unique_ptr<Material> create_lambert(const std::string& name) {
            auto material = std::make_unique<Material>(name);
            material->set_builtin_material_type(0);
            material->set_diffuse_color(glm::vec3(0.8f));
            material->set_ambient_color(glm::vec3(0.1f));
            material->set_uv_tiling(1.0f, 1.0f);  
            material->set_uv_offset(glm::vec2(0.0f, 0.0f)); 
            material->set_uv_rotation(0.0f);     
            return material;
        }
        
        static std::unique_ptr<Material> create_phong(const std::string& name) {
            auto material = std::make_unique<Material>(name);
            material->set_builtin_material_type(1);
            material->set_diffuse_color(glm::vec3(0.8f));
            material->set_ambient_color(glm::vec3(0.3f));
            material->set_specular_color(glm::vec3(0.5f));
            material->set_shininess(32.0f);
            material->set_uv_tiling(1.0f, 1.0f);  
            material->set_uv_offset(glm::vec2(0.0f, 0.0f)); 
            material->set_uv_rotation(0.0f);     
            return material;
        }
        
        static std::unique_ptr<Material> create_pbr(const std::string& name) {
            auto material = std::make_unique<Material>(name);
            material->set_builtin_material_type(2);
            material->set_diffuse_color(glm::vec3(0.8f));
            material->set_property("metallic", 0.0f);
            material->set_property("roughness", 0.5f);
            material->set_property("ao", 1.0f);
            material->set_uv_tiling(1.0f, 1.0f);  
            material->set_uv_offset(glm::vec2(0.0f, 0.0f)); 
            material->set_uv_rotation(0.0f);     
            return material;
        }
        
        // Create material with custom shader
        static std::unique_ptr<Material> create_custom(const std::string& name, 
                                                      const std::string& vertex_path,
                                                      const std::string& fragment_path) {
            auto material = std::make_unique<Material>(name);
            material->set_custom_shader(vertex_path, fragment_path);
            return material;
        }
        
        // Create material from shader template
        static std::unique_ptr<Material> create_from_template(const std::string& name,
                                                             const std::string& template_name) {
            auto material = std::make_unique<Material>(name);
            
            // Built-in templates
            if (template_name == "toon") {
                material->set_custom_shader("assets/shaders/toon.vert", "assets/shaders/toon.frag");
                material->set_property("toonSteps", 4.0f);
                material->set_property("outlineWidth", 0.02f);
            }
            else if (template_name == "water") {
                material->set_custom_shader("assets/shaders/water.vert", "assets/shaders/water.frag");
                material->set_property("waveSpeed", 1.0f);
                material->set_property("waveAmplitude", 0.1f);
                material->add_shader_define("ANIMATED_WAVES");
            }
            else if (template_name == "hologram") {
                material->set_custom_shader("assets/shaders/hologram.vert", "assets/shaders/hologram.frag");
                material->set_property("scanlineSpeed", 2.0f);
                material->set_property("glitchIntensity", 0.1f);
                material->set_property("hologramColor", glm::vec3(0.0f, 1.0f, 1.0f));
            }
            
            return material;
        }
    };
}
