//
// Created by denzel on 08/12/2025.
//

#include "MaterialSerializer.h"

#include <fstream>
#include <iostream>

#include "hellfire/utilities/SerializerUtils.h"

namespace hellfire {
    bool MaterialSerializer::save(const std::filesystem::path &filepath, const MaterialData &mat) {
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            std::cerr << "MaterialSerializer: Cannot open file for writing: " << filepath << std::endl;
            return false;
        }

        // Header
        write_header(file, MAGIC, VERSION);

        // Name
        write_binary_string(file, mat.name);

        // Colors (write as raw floats)
        write_binary(file, mat.diffuse_color);
        write_binary(file, mat.ambient_color);
        write_binary(file, mat.specular_color);
        write_binary(file, mat.emissive_color);

        // Scalars
        write_binary(file, mat.opacity);
        write_binary(file, mat.shininess);
        write_binary(file, mat.metallic);
        write_binary(file, mat.roughness);

        // UV transforms
        write_binary(file, mat.uv_scale);
        write_binary(file, mat.uv_offset);

        // Flags
        write_binary(file, mat.double_sided);
        write_binary(file, mat.alpha_blend);
        write_binary(file, mat.alpha_cutoff);

        // Texture references
        const uint32_t texture_count = static_cast<uint32_t>(mat.texture_assets.size());
        write_binary(file, texture_count);

        for (const auto &[type, asset_id]: mat.texture_assets) {
            write_binary(file, static_cast<uint32_t>(type));
            write_binary(file, asset_id);
        }

        return file.good();
    }

    std::optional<MaterialData> MaterialSerializer::load(const std::filesystem::path &filepath) {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            std::cerr << "MaterialSerializer: Cannot open file: " << filepath << std::endl;
            return std::nullopt;
        }

        // Validate header
        uint32_t version;
        if (!read_and_validate_header(file, MAGIC, VERSION, version)) {
            std::cerr << "MaterialSerializer: Invalid file header: " << filepath << std::endl;
            return std::nullopt;
        }

        MaterialData mat;

        // Name
        if (!read_binary_string(file, mat.name)) return std::nullopt;

        // Colors
        if (!read_binary(file, mat.diffuse_color)) return std::nullopt;
        if (!read_binary(file, mat.ambient_color)) return std::nullopt;
        if (!read_binary(file, mat.specular_color)) return std::nullopt;
        if (!read_binary(file, mat.emissive_color)) return std::nullopt;

        // Scalars
        if (!read_binary(file, mat.opacity)) return std::nullopt;
        if (!read_binary(file, mat.shininess)) return std::nullopt;
        if (!read_binary(file, mat.metallic)) return std::nullopt;
        if (!read_binary(file, mat.roughness)) return std::nullopt;

        // UV transforms
        if (!read_binary(file, mat.uv_scale)) return std::nullopt;
        if (!read_binary(file, mat.uv_offset)) return std::nullopt;

        // Flags
        if (!read_binary(file, mat.double_sided)) return std::nullopt;
        if (!read_binary(file, mat.alpha_blend)) return std::nullopt;
        if (!read_binary(file, mat.alpha_cutoff)) return std::nullopt;

        // Texture references
        uint32_t texture_count;
        if (!read_binary(file, texture_count)) return std::nullopt;

        for (uint32_t i = 0; i < texture_count; i++) {
            uint32_t type_raw;
            AssetID asset_id;

            if (!read_binary(file, type_raw)) return std::nullopt;
            if (!read_binary(file, asset_id)) return std::nullopt;

            mat.texture_assets[static_cast<TextureType>(type_raw)] = asset_id;
        }

        return mat;
    }

    // Helper for TextureType to string conversion
    static const char *texture_type_to_string(TextureType type) {
        switch (type) {
            case TextureType::DIFFUSE: return "diffuse";
            case TextureType::NORMAL: return "normal";
            case TextureType::SPECULAR: return "specular";
            case TextureType::METALNESS: return "metalness";
            case TextureType::ROUGHNESS: return "roughness";
            case TextureType::AMBIENT_OCCLUSION: return "ambient_occlusion";
            case TextureType::EMISSIVE: return "emissive";
            default: return "unknown";
        }
    }

    static TextureType string_to_texture_type(const std::string &str) {
        if (str == "diffuse") return TextureType::DIFFUSE;
        if (str == "normal") return TextureType::NORMAL;
        if (str == "specular") return TextureType::SPECULAR;
        if (str == "metalness") return TextureType::METALNESS;
        if (str == "roughness") return TextureType::ROUGHNESS;
        if (str == "ambient_occlusion") return TextureType::AMBIENT_OCCLUSION;
        if (str == "emissive") return TextureType::EMISSIVE;
        return TextureType::DIFFUSE; // fallback
    }

    bool MaterialSerializer::save_json(const std::filesystem::path &filepath, const MaterialData &mat) {
        nlohmann::json j;

        j["version"] = VERSION;
        j["name"] = mat.name;

        j["colors"] = {
            {"diffuse", vec3_to_json(mat.diffuse_color)},
            {"ambient", vec3_to_json(mat.ambient_color)},
            {"specular", vec3_to_json(mat.specular_color)},
            {"emissive", vec3_to_json(mat.emissive_color)}
        };

        j["properties"] = {
            {"opacity", mat.opacity},
            {"shininess", mat.shininess},
            {"metallic", mat.metallic},
            {"roughness", mat.roughness}
        };

        j["uv"] = {
            {"scale", vec2_to_json(mat.uv_scale)},
            {"offset", vec2_to_json(mat.uv_offset)}
        };

        j["flags"] = {
            {"double_sided", mat.double_sided},
            {"alpha_blend", mat.alpha_blend},
            {"alpha_cutoff", mat.alpha_cutoff}
        };

        // Textures
        nlohmann::json textures = nlohmann::json::object();
        for (const auto &[type, asset_id]: mat.texture_assets) {
            textures[texture_type_to_string(type)] = asset_id;
        }
        j["textures"] = textures;

        std::ofstream file(filepath);
        if (!file) return false;

        file << j.dump(2);
        return file.good();
    }

    std::optional<MaterialData> MaterialSerializer::load_json(const std::filesystem::path &filepath) {
        std::ifstream file(filepath);
        if (!file) return std::nullopt;

        try {
            nlohmann::json j;
            file >> j;

            MaterialData mat;

            mat.name = j.value("name", "Material");

            // Colors
            if (j.contains("colors")) {
                const auto &colors = j["colors"];
                if (auto v = json_get_vec3(colors, "diffuse")) mat.diffuse_color = *v;
                if (auto v = json_get_vec3(colors, "ambient")) mat.ambient_color = *v;
                if (auto v = json_get_vec3(colors, "specular")) mat.specular_color = *v;
                if (auto v = json_get_vec3(colors, "emissive")) mat.emissive_color = *v;
            }

            // Properties
            if (j.contains("properties")) {
                const auto &props = j["properties"];
                mat.opacity = props.value("opacity", 1.0f);
                mat.shininess = props.value("shininess", 32.0f);
                mat.metallic = props.value("metallic", 0.0f);
                mat.roughness = props.value("roughness", 0.5f);
            }

            // UV
            if (j.contains("uv")) {
                const auto &uv = j["uv"];
                if (auto v = json_get_vec2(uv, "scale")) mat.uv_scale = *v;
                if (auto v = json_get_vec2(uv, "offset")) mat.uv_offset = *v;
            }

            // Flags
            if (j.contains("flags")) {
                const auto &flags = j["flags"];
                mat.double_sided = flags.value("double_sided", false);
                mat.alpha_blend = flags.value("alpha_blend", false);
                mat.alpha_cutoff = flags.value("alpha_cutoff", 0.5f);
            }

            // Textures
            if (j.contains("textures") && j["textures"].is_object()) {
                for (const auto &[key, value]: j["textures"].items()) {
                    TextureType type = string_to_texture_type(key);
                    AssetID asset_id = value.get<AssetID>();
                    mat.texture_assets[type] = asset_id;
                }
            }

            return mat;
        } catch (const std::exception &e) {
            std::cerr << "MaterialSerializer: JSON parse error: " << e.what() << std::endl;
            return std::nullopt;
        }
    }
}
