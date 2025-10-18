#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <optional>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include "MaterialConstants.h"
#include "hellfire/graphics/texture/Texture.h"

namespace hellfire {
    class MaterialInstance;
    class Application;
    class ShaderManager;
}

namespace hellfire {
    class Material : public std::enable_shared_from_this<Material> {
    public:
        enum class PropertyType {
            FLOAT, VEC2, VEC3, COLOR3, VEC4, COLOR4, TEXTURE, TEXTURE_FLAG, BOOL, INT, MAT3, MAT4
        };

        struct Property {
            PropertyType type;
            std::string name;
            std::variant<float, glm::vec2, glm::vec3, glm::vec4, Texture *, bool, int, glm::mat3, glm::mat4> value;

            std::string uniform_name;

            Property() {
            }

            Property(const std::string &name, float value, const std::string &uniform = "") : type(PropertyType::FLOAT),
                name(name), value(value), uniform_name(uniform.empty() ? name : uniform) {
            }

            Property(const std::string &name, const glm::vec2 &value,
                     const std::string &uniform = "") : type(PropertyType::VEC2), value(value),
                                                        uniform_name(uniform.empty() ? name : uniform), name(name) {
            }

            Property(const std::string &name, const glm::vec3 &value,
                     const std::string &uniform = "") : type(PropertyType::VEC3), value(value),
                                                        uniform_name(uniform.empty() ? name : uniform), name(name) {
            }

            Property(const std::string &name, const glm::vec3 &value, PropertyType type,
                     const std::string &uniform = "")
                : type(type), name(name), value(value),
                  uniform_name(uniform.empty() ? name : uniform) {
            }

            Property(const std::string &name, const glm::vec4 &value, PropertyType type,
                     const std::string &uniform = "")
                : type(type), name(name), value(value),
                  uniform_name(uniform.empty() ? name : uniform) {
            }

            Property(const std::string &name, const glm::vec4 &value, const std::string &uniform = "") : name(name),
                type(PropertyType::VEC4), value(value), uniform_name(uniform.empty() ? name : uniform) {
            }

            Property(const std::string &name, Texture *value, const std::string &uniform = "") : name(name),
                type(PropertyType::TEXTURE), value(value), uniform_name(uniform.empty() ? name : uniform) {
            }

            Property(const std::string &name, bool value, const std::string &uniform = "") : name(name),
                type(PropertyType::BOOL), value(value), uniform_name(uniform.empty() ? name : uniform) {
            }

            Property(const std::string &name, int value, const std::string &uniform = "") : name(name),
                type(PropertyType::INT), value(value), uniform_name(uniform.empty() ? name : uniform) {
            }

            Property(const std::string &name, const glm::mat3 &value, const std::string &uniform = "") : name(name),
                type(PropertyType::MAT3), value(value), uniform_name(uniform.empty() ? name : uniform) {
            }

            Property(const std::string &name, const glm::mat4 &value, const std::string &uniform = "") : name(name),
                type(PropertyType::MAT4), value(value), uniform_name(uniform.empty() ? name : uniform) {
            }
        };

        struct ShaderInfo {
            std::string vertex_path;
            std::string fragment_path;
            std::optional<std::string> geometry_path;
            std::unordered_set<std::string> defines;
            std::unordered_map<std::string, std::string> uniform_mappings; // Property name -> uniform name

            bool is_valid() const {
                return !vertex_path.empty() && !fragment_path.empty();
            }
        };

    private:
        std::string name_;
        std::map<std::string, Property> properties_;
        std::optional<ShaderInfo> custom_shader_info_;
        uint32_t compiled_shader_id_ = 0;

        // Instancing support
        std::shared_ptr<Material> base_material_;
        std::unordered_map<std::string, Property> overrides_;
        mutable std::unordered_set<std::string> touched_uniforms_;

    public:
        explicit Material(const std::string &name) : name_(name) {}

        // Generic Property Setters
        template<typename T>
        void set_property(const std::string &name, const T &value, PropertyType type,
                          const std::string &uniform_name = "") {
            properties_[name] = Property(name, value, type, uniform_name);
        }

        template<typename T>
        void set_property(const std::string &name, const T &value, const std::string &uniform_name = "") {
            properties_[name] = Property(name, value, uniform_name);
        }

        // Generic Property Getter
        template<typename T>
        T get_property(const std::string &name, const T &default_value = T{}) const {
            if (const auto it = properties_.find(name); it != properties_.end()) {
                if (auto *val = std::get_if<T>(&it->second.value)) {
                    return *val;
                }
            }
            return default_value;
        }

        // Texture Management
        Material& set_texture(const std::string &path, TextureType type, int texture_slot = 0) {
            auto* texture = new Texture(path, type);
            return set_texture_internal(texture, type, texture_slot);
        }
        
        Material& set_texture(const std::shared_ptr<Texture> &texture, int texture_slot = 0) {
            return set_texture_internal(texture.get(), texture->get_type(), texture_slot);
        }
        
        Material& set_texture(Texture* texture, int texture_slot = 0) {
            return set_texture_internal(texture, texture->get_type(), texture_slot);
        }

        // Color Setters
        void set_diffuse_color(const glm::vec3 &color) { 
            set_property(MaterialConstants::DIFFUSE_COLOR, color, PropertyType::COLOR3); 
        }
        
        void set_ambient_color(const glm::vec3 &color) { 
            set_property(MaterialConstants::AMBIENT_COLOR, color, PropertyType::COLOR3); 
        }
        
        void set_specular_color(const glm::vec3 &color) { 
            set_property(MaterialConstants::SPECULAR_COLOR, color, PropertyType::COLOR3); 
        }
        
        void set_emissive_color(const glm::vec3 &color) { 
            set_property(MaterialConstants::EMISSIVE_COLOR, color, PropertyType::COLOR3); 
        }

        //  Material Property Setters 
        void set_shininess(float shininess) { 
            set_property(MaterialConstants::SHININESS, shininess); 
        }

        void set_metallic(float metallic) { 
            set_property(MaterialConstants::METALLIC, metallic); 
        }
        
        void set_roughness(float roughness) { 
            set_property(MaterialConstants::ROUGHNESS, roughness); 
        }
        
        void set_opacity(float opacity) {
            set_property(MaterialConstants::OPACITY, glm::clamp(opacity, 0.0f, 1.0f));
        }

        bool is_transparent() const {
            return get_property<float>(MaterialConstants::OPACITY, 1.0f) < 1.0f;
        }

        // UV Transform
        void set_uv_tiling(const glm::vec2 &tiling) {
            set_property(MaterialConstants::UV_TILING, tiling);
        }

        void set_uv_tiling(float x, float y) {
            set_uv_tiling(glm::vec2(x, y));
        }

        void set_uv_offset(const glm::vec2 &offset) {
            set_property(MaterialConstants::UV_OFFSET, offset);
        }

        void set_uv_rotation(float rotation) {
            set_property(MaterialConstants::UV_ROTATION, rotation);
        }

        // Custom Shader Support
        void set_custom_shader(const std::string &vertex_path, const std::string &fragment_path) {
            custom_shader_info_ = ShaderInfo{vertex_path, fragment_path};
        }

        void set_custom_shader(const ShaderInfo &shader_info) {
            custom_shader_info_ = shader_info;
        }

        void add_shader_define(const std::string &define) {
            if (custom_shader_info_) {
                custom_shader_info_->defines.insert(define);
            }
        }

        void set_uniform_mapping(const std::string &property_name, const std::string &uniform_name) {
            if (custom_shader_info_) {
                custom_shader_info_->uniform_mappings[property_name] = uniform_name;
            }
        }

        bool has_custom_shader() const {
            return custom_shader_info_ && custom_shader_info_->is_valid();
        }

        const ShaderInfo *get_shader_info() const {
            return custom_shader_info_ ? &*custom_shader_info_ : nullptr;
        }

        void set_compiled_shader_id(uint32_t shader_id) {
            compiled_shader_id_ = shader_id;
        }

        uint32_t get_compiled_shader_id() const {
            return compiled_shader_id_;
        }

        /// Used to bind a Material for rendering
        void bind() const;
        void unbind() const;

        // Getters 
        const auto &get_properties() const { return properties_; }
        const std::string &get_name() const { return name_; }
        void set_name(const std::string &name) { name_ = name; }
    private:
        Material& set_texture_internal(Texture* texture, TextureType type, int texture_slot) {
            const char* uniform_name = MaterialConstants::get_texture_uniform_name(type);
            const char* flag_name = MaterialConstants::get_texture_flag_name(type);

            if (!uniform_name || !flag_name) {
                std::cerr << "Warning: Unsupported texture type" << std::endl;
                return *this;
            }

            if (texture_slot >= 0) {
                texture->set_slot(texture_slot);
                set_property(uniform_name + std::string("Slot"), texture_slot);
            }

            set_property(uniform_name, texture, uniform_name);
            set_property(flag_name, texture != nullptr);

            return *this;
        }

        void bind_all_properties(uint32_t shader_program, int &texture_unit) const;

        bool has_property(const std::string &name) const {
            return properties_.find(name) != properties_.end();
        }

        Property get_property_object(const std::string &name) const {
            if (const auto it = properties_.find(name); it != properties_.end()) {
                return it->second;
            }
            return Property{}; // Return default property
        }
    };

    class MaterialBuilder {
    public:
        static std::shared_ptr<Material> create(const std::string &name);
        static std::shared_ptr<Material> create_custom(const std::string &name,
                                                       const std::string &vertex_path,
                                                       const std::string &fragment_path);
    private:
        static void compile_shader_from_material(Material &material);
    };
}
