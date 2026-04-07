//
// Created by denzel on 08/12/2025.
//

#include "ModelSerializer.h"

#include <fstream>
#include <iostream>

#include "hellfire/utilities/SerializerUtils.h"
#include "glm/glm.hpp"

namespace hellfire {
    bool ModelSerializer::save(const std::filesystem::path &filepath, const ImportResult &result) {
        if (!result.success) {
            std::cerr << "ModelSerializer: Cannot save failed import result" << std::endl;
            return false;
        }

        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            std::cerr << "ModelSerializer: Cannot open file for writing: " << filepath << std::endl;
            return false;
        }

        // Header
        write_header(file, MAGIC, VERSION);

        // Root node index
        write_binary(file, static_cast<uint32_t>(result.root_node_index));

        // Nodes
        const uint32_t node_count = static_cast<uint32_t>(result.nodes.size());
        write_binary(file, node_count);

        for (const auto& node : result.nodes) {
            write_binary_string(file, node.name);
            write_binary(file, node.position);
            write_binary(file, node.rotation);
            write_binary(file, node.scale);

            // Mesh indices
            const uint32_t mesh_idx_count = static_cast<uint32_t>(node.mesh_indices.size());
            write_binary(file, mesh_idx_count);
            for (size_t idx : node.mesh_indices) {
                write_binary(file, static_cast<uint32_t>(idx));
            }

            // Child indices
            const uint32_t child_idx_count = static_cast<uint32_t>(node.child_indices.size());
            write_binary(file, child_idx_count);
            for (size_t idx : node.child_indices) {
                write_binary(file, static_cast<uint32_t>(idx));
            }
        }

        // Meshes
        const uint32_t mesh_count = static_cast<uint32_t>(result.meshes.size());
        write_binary(file, mesh_count);

        for (const auto& mesh : result.meshes) {
            write_binary_string(file, mesh.name);
            write_binary(file, mesh.mesh_asset);
            write_binary(file, mesh.material_asset);
        }

        return file.good();
    }

    std::optional<ImportResult> ModelSerializer::load(const std::filesystem::path &filepath) {
                std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            std::cerr << "ModelSerializer: Cannot open file: " << filepath << std::endl;
            return std::nullopt;
        }

        // Validate header
        uint32_t version;
        if (!read_and_validate_header(file, MAGIC, VERSION, version)) {
            std::cerr << "ModelSerializer: Invalid file header: " << filepath << std::endl;
            return std::nullopt;
        }

        ImportResult result;
        result.success = true;

        // Root node index
        uint32_t root_idx;
        if (!read_binary(file, root_idx)) return std::nullopt;
        result.root_node_index = root_idx;

        // Nodes
        uint32_t node_count;
        if (!read_binary(file, node_count)) return std::nullopt;
        result.nodes.resize(node_count);

        for (auto& node : result.nodes) {
            if (!read_binary_string(file, node.name)) return std::nullopt;
            if (!read_binary(file, node.position)) return std::nullopt;
            if (!read_binary(file, node.rotation)) return std::nullopt;
            if (!read_binary(file, node.scale)) return std::nullopt;

            // Mesh indices
            uint32_t mesh_idx_count;
            if (!read_binary(file, mesh_idx_count)) return std::nullopt;
            node.mesh_indices.resize(mesh_idx_count);
            for (uint32_t i = 0; i < mesh_idx_count; i++) {
                uint32_t idx;
                if (!read_binary(file, idx)) return std::nullopt;
                node.mesh_indices[i] = idx;
            }

            // Child indices
            uint32_t child_idx_count;
            if (!read_binary(file, child_idx_count)) return std::nullopt;
            node.child_indices.resize(child_idx_count);
            for (uint32_t i = 0; i < child_idx_count; i++) {
                uint32_t idx;
                if (!read_binary(file, idx)) return std::nullopt;
                node.child_indices[i] = idx;
            }
        }

        // Meshes
        uint32_t mesh_count;
        if (!read_binary(file, mesh_count)) return std::nullopt;
        result.meshes.resize(mesh_count);

        for (auto& mesh : result.meshes) {
            if (!read_binary_string(file, mesh.name)) return std::nullopt;
            if (!read_binary(file, mesh.mesh_asset)) return std::nullopt;
            if (!read_binary(file, mesh.material_asset)) return std::nullopt;
        }

        return result;
    }

    bool ModelSerializer::save_json(const std::filesystem::path &filepath, const ImportResult &result) {
        if (!result.success) return false;

        nlohmann::json j;
        j["version"] = VERSION;
        j["root_node_index"] = result.root_node_index;

        // Nodes
        auto& nodes_json = j["nodes"];
        for (const auto& node : result.nodes) {
            nlohmann::json node_json;
            node_json["name"] = node.name;
            node_json["position"] = vec3_to_json(node.position);
            node_json["rotation"] = vec3_to_json(node.rotation);
            node_json["scale"] = vec3_to_json(node.scale);
            node_json["mesh_indices"] = node.mesh_indices;
            node_json["child_indices"] = node.child_indices;
            nodes_json.push_back(node_json);
        }

        // Meshes
        auto& meshes_json = j["meshes"];
        for (const auto& mesh : result.meshes) {
            meshes_json.push_back({
                {"name", mesh.name},
                {"mesh_asset", mesh.mesh_asset},
                {"material_asset", mesh.material_asset}
            });
        }

        std::ofstream file(filepath);
        if (!file) return false;

        file << j.dump(2);
        return file.good();
    }

    std::optional<ImportResult> ModelSerializer::load_json(const std::filesystem::path &filepath) {
        std::ifstream file(filepath);
        if (!file) return std::nullopt;

        try {
            nlohmann::json j;
            file >> j;

            ImportResult result;
            result.success = true;
            result.root_node_index = j.value("root_node_index", 0);

            // Nodes
            for (const auto& node_json : j["nodes"]) {
                ImportedNode node;
                node.name = node_json.value("name", "Node");

                if (auto v = json_get_vec3(node_json, "position")) node.position = *v;
                if (auto v = json_get_vec3(node_json, "rotation")) node.rotation = *v;
                if (auto v = json_get_vec3(node_json, "scale")) node.scale = *v;
                else node.scale = glm::vec3(1.0f);

                node.mesh_indices = node_json["mesh_indices"].get<std::vector<size_t>>();
                node.child_indices = node_json["child_indices"].get<std::vector<size_t>>();

                result.nodes.push_back(node);
            }

            // Meshes
            for (const auto& mesh_json : j["meshes"]) {
                ImportedMesh mesh;
                mesh.name = mesh_json.value("name", "Mesh");
                mesh.mesh_asset = mesh_json.value("mesh_asset", INVALID_ASSET_ID);
                mesh.material_asset = mesh_json.value("material_asset", INVALID_ASSET_ID);
                result.meshes.push_back(mesh);
            }

            return result;
        } catch (const std::exception& e) {
            std::cerr << "ModelSerializer: JSON parse error: " << e.what() << std::endl;
            return std::nullopt;
        }
    }
} // hellfire