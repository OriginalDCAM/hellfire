//
// Created by denzel on 08/12/2025.
//

#include "hellfire/graphics/Vertex.h"
#include "MeshSerializer.h"

#include <fstream>

#include "hellfire/utilities/SerializerUtils.h"

namespace hellfire {
    bool MeshSerializer::save(const std::filesystem::path &filepath, const Mesh &mesh) {
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            std::cerr << "MeshSerializer: Cannot open file for writing: " << filepath << std::endl;
            return false;
        }

        // Header
        if (!write_header(file, MAGIC, VERSION)) {
            return false;
        }

        // Mesh flags
        write_binary(file, mesh.is_wireframe);

        // Vertex data
        write_vertex_vector(file, mesh.vertices);

        // Index data
        write_binary_vector(file, mesh.indices);

        return file.good();
    }

    std::shared_ptr<Mesh> MeshSerializer::load(const std::filesystem::path &filepath) {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            std::cerr << "MeshSerializer: Cannot open file: " << filepath << std::endl;
            return nullptr;
        }

        // Validate header
        uint32_t version;
        if (!read_and_validate_header(file, MAGIC, VERSION, version)) {
            std::cerr << "MeshSerializer: Invalid file header: " << filepath << std::endl;
            return nullptr;
        }

        auto mesh = std::make_shared<Mesh>();

        // Mesh flags
        if (!read_binary(file, mesh->is_wireframe)) {
            return nullptr;
        }

        // Vertex data
        if (!read_vertex_vector(file, mesh->vertices)) {
            return nullptr;
        }

        // Index data
        if (!read_binary_vector(file, mesh->indices)) {
            return nullptr;
        }

        return mesh;
    }

    bool MeshSerializer::save_json(const std::filesystem::path &filepath, const Mesh &mesh) {
        nlohmann::json j;
        j["version"] = VERSION;
        j["is_wireframe"] = mesh.is_wireframe;

        // Vertices
        auto &verts = j["vertices"];
        for (const auto &v: mesh.vertices) {
            verts.push_back({
                {"position", vec3_to_json(v.position)},
                {"normal", vec3_to_json(v.normal)},
                {"texCoords", vec2_to_json(v.texCoords)},
                {"color", vec3_to_json(v.color)},
                {"tangent", vec3_to_json(v.tangent)},
                {"bitangent", vec3_to_json(v.bitangent)}
            });
        }

        j["indices"] = mesh.indices;

        std::ofstream file(filepath);
        if (!file) return false;

        file << j.dump(2);
        return file.good();
    }

    std::shared_ptr<Mesh> MeshSerializer::load_json(const std::filesystem::path &filepath) {
        std::ifstream file(filepath);
        if (!file) return nullptr;

        try {
            nlohmann::json j;
            file >> j;

            auto mesh = std::make_shared<Mesh>();
            mesh->is_wireframe = j.value("is_wireframe", false);

            for (const auto &v: j["vertices"]) {
                Vertex vert{};

                if (auto pos = json_get_vec3(v, "position")) vert.position = *pos;
                if (auto norm = json_get_vec3(v, "normal")) vert.normal = *norm;
                if (auto tex = json_get_vec2(v, "texCoords")) vert.texCoords = *tex;
                if (auto col = json_get_vec3(v, "color")) vert.color = *col;
                if (auto tan = json_get_vec3(v, "tangent")) vert.tangent = *tan;
                if (auto bitan = json_get_vec3(v, "bitangent")) vert.bitangent = *bitan;

                mesh->vertices.push_back(vert);
            }

            mesh->indices = j["indices"].get<std::vector<unsigned int> >();

            return mesh;
        } catch (const std::exception &e) {
            std::cerr << "MeshSerializer: JSON parse error: " << e.what() << std::endl;
            return nullptr;
        }
    }
}
