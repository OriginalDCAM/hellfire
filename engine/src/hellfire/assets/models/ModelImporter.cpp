//
// Created by denzel on 08/12/2025.
//

#include "ModelImporter.h"


#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "glm/detail/type_mat4x4.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "hellfire/graphics/Mesh.h"
#include "hellfire/graphics/Vertex.h"
#include "hellfire/graphics/material/MaterialData.h"
#include "hellfire/serializers/MaterialSerializer.h"
#include "hellfire/serializers/MeshSerializer.h"

namespace hellfire {
    ModelImporter::ModelImporter(AssetRegistry &registry,
                                 const std::filesystem::path &output_dir) : registry_(registry),
                                                                            output_dir_(output_dir) {
        create_directories(output_dir_);
    }

    ImportResult ModelImporter::import(const std::filesystem::path &source_path, const ImportSettings &settings) {
        ImportResult result;

        source_path_ = source_path;
        source_dir_ = source_path.parent_path();
        base_name_ = source_path.stem().string();

        Assimp::Importer importer;
        ai_scene_ = importer.ReadFile(source_path.string(), build_import_flags(settings));

        if (!ai_scene_ || !ai_scene_->mRootNode) {
            result.error_message = importer.GetErrorString();
            return result;
        }

        // Process hierarchy starting from root
        process_node_hierarchy(ai_scene_->mRootNode, result);

        result.success = true;
        ai_scene_ = nullptr;

        return result;
    }

    unsigned int ModelImporter::build_import_flags(const ImportSettings &settings) const {
        unsigned int flags = aiProcess_ValidateDataStructure;

        if (settings.triangulate)
            flags |= aiProcess_Triangulate;
        if (settings.generate_normals)
            flags |= aiProcess_GenSmoothNormals;
        if (settings.generate_tangents)
            flags |= aiProcess_CalcTangentSpace;
        if (settings.flip_uvs)
            flags |= aiProcess_FlipUVs;
        if (settings.optimize_meshes)
            flags |= aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph;

        flags |= aiProcess_JoinIdenticalVertices;
        flags |= aiProcess_ImproveCacheLocality;

        return flags;
    }

    void ModelImporter::process_node_hierarchy(const aiNode *node, ImportResult &result, size_t parent_index) {
        const size_t current_index = result.nodes.size();
        result.nodes.push_back(convert_node(node));
        auto &imported_node = result.nodes.back();

        // Link to parent
        if (parent_index != SIZE_MAX) {
            result.nodes[parent_index].child_indices.push_back(current_index);
        } else {
            result.root_node_index = current_index;
        }

        // Process meshes attached to this node
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            const unsigned int mesh_idx = node->mMeshes[i];
            const aiMesh *ai_mesh = ai_scene_->mMeshes[mesh_idx];

            ImportedMesh imported_mesh;
            imported_mesh.name = ai_mesh->mName.length > 0
                                     ? ai_mesh->mName.C_Str()
                                     : make_unique_name(base_name_, "mesh", mesh_idx);

            // Process and serialize mesh
            imported_mesh.mesh_asset = process_mesh(ai_mesh, mesh_idx);
            result.created_mesh_assets.push_back(imported_mesh.mesh_asset);

            // Process and serialize material
            if (ai_mesh->mMaterialIndex < ai_scene_->mNumMaterials) {
                const aiMaterial *material = ai_scene_->mMaterials[ai_mesh->mMaterialIndex];
                imported_mesh.material_asset = process_material(material, ai_mesh->mMaterialIndex);

                if (imported_mesh.material_asset != INVALID_ASSET_ID) {
                    result.created_material_assets.push_back(imported_mesh.material_asset);
                }
            }

            imported_node.mesh_indices.push_back(result.meshes.size());
            result.meshes.push_back(imported_mesh);
        }

        // Recurse into children
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            process_node_hierarchy(node->mChildren[i], result, current_index);
        }
    }

    ImportedNode ModelImporter::convert_node(const aiNode *node) const {
        ImportedNode result;

        result.name = node->mName.length > 0 ? node->mName.C_Str() : "Node";

        glm::mat4 transform = convert_matrix(node->mTransformation);
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::quat rotation;

        decompose(transform, result.scale, rotation, result.position, skew, perspective);
        result.rotation = eulerAngles(rotation);

        return result;
    }

    AssetID ModelImporter::process_mesh(const aiMesh *ai_mesh, size_t mesh_index) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        vertices.reserve(ai_mesh->mNumVertices);
        indices.reserve(ai_mesh->mNumFaces * 3);

        // Extract vertices
        for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++) {
            Vertex v{};

            v.position = {
                ai_mesh->mVertices[i].x,
                ai_mesh->mVertices[i].y,
                ai_mesh->mVertices[i].z
            };

            if (ai_mesh->HasNormals()) {
                v.normal = {
                    ai_mesh->mNormals[i].x,
                    ai_mesh->mNormals[i].y,
                    ai_mesh->mNormals[i].z
                };
            }

            if (ai_mesh->HasTangentsAndBitangents()) {
                v.tangent = {
                    ai_mesh->mTangents[i].x,
                    ai_mesh->mTangents[i].y,
                    ai_mesh->mTangents[i].z
                };
                v.bitangent = {
                    ai_mesh->mBitangents[i].x,
                    ai_mesh->mBitangents[i].y,
                    ai_mesh->mBitangents[i].z
                };
            }

            if (ai_mesh->HasTextureCoords(0)) {
                v.texCoords = {
                    ai_mesh->mTextureCoords[0][i].x,
                    ai_mesh->mTextureCoords[0][i].y
                };
            }

            if (ai_mesh->HasVertexColors(0)) {
                v.color = {
                    ai_mesh->mColors[0][i].r,
                    ai_mesh->mColors[0][i].g,
                    ai_mesh->mColors[0][i].b
                };
            } else {
                v.color = glm::vec3(1.0f);
            }

            vertices.push_back(v);
        }

        // Extract indices
        for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++) {
            const aiFace &face = ai_mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Serialize to file
        Mesh mesh(vertices, indices);
        const std::string filename = make_unique_name(base_name_, "mesh", mesh_index) + ".hfmesh";
        const auto filepath = output_dir_ / filename;


        if (!MeshSerializer::save(filepath, mesh)) {
            std::cerr << "Failed to save mesh: " << filepath << std::endl;
            return INVALID_ASSET_ID;
        }

        return registry_.register_asset(filepath, AssetType::MESH);
    }

    AssetID ModelImporter::process_material(const aiMaterial *ai_mat, size_t material_index) {
        MaterialData data;

        // Get name
        aiString name;
        if (ai_mat->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) {
            data.name = name.C_Str();
        } else {
            data.name = make_unique_name(base_name_, "material", material_index);
        }

        // Colors
        aiColor3D color;
        if (ai_mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
            data.diffuse_color = {color.r, color.g, color.b};
        if (ai_mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
            data.ambient_color = {color.r, color.g, color.b};
        if (ai_mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
            data.specular_color = {color.r, color.g, color.b};
        if (ai_mat->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
            data.emissive_color = {color.r, color.g, color.b};

        // Scalars
        float value;
        if (ai_mat->Get(AI_MATKEY_OPACITY, value) == AI_SUCCESS)
            data.opacity = value;
        if (ai_mat->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS)
            data.shininess = std::max(value, 1.0f);
        if (ai_mat->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS)
            data.metallic = value;
        if (ai_mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS)
            data.roughness = value;

        // Textures
        auto try_load_texture = [&](aiTextureType ai_type, TextureType hf_type) {
            if (ai_mat->GetTextureCount(ai_type) == 0) return;

            aiString tex_path;
            if (ai_mat->GetTexture(ai_type, 0, &tex_path) != AI_SUCCESS) return;

            AssetID tex_asset = process_texture(tex_path.C_Str(), hf_type);
            if (tex_asset != INVALID_ASSET_ID) {
                data.texture_assets[hf_type] = tex_asset;
            }
        };

        try_load_texture(aiTextureType_DIFFUSE, TextureType::DIFFUSE);
        try_load_texture(aiTextureType_NORMALS, TextureType::NORMAL);
        try_load_texture(aiTextureType_SPECULAR, TextureType::SPECULAR);
        try_load_texture(aiTextureType_METALNESS, TextureType::METALNESS);
        try_load_texture(aiTextureType_DIFFUSE_ROUGHNESS, TextureType::ROUGHNESS);
        try_load_texture(aiTextureType_AMBIENT_OCCLUSION, TextureType::AMBIENT_OCCLUSION);
        try_load_texture(aiTextureType_EMISSIVE, TextureType::EMISSIVE);

        // Serialize
        const std::string filename = data.name + ".hfmat";
        const auto filepath = output_dir_ / filename;

        if (!MaterialSerializer::save(filepath, data)) {
            std::cerr << "Failed to save material: " << filepath << std::endl;
            return INVALID_ASSET_ID;
        }

        return registry_.register_asset(filepath, AssetType::MATERIAL);
    }

    AssetID ModelImporter::process_texture(const std::string &texture_ref, TextureType type) {
        std::filesystem::path resolved_path;

        if (is_embedded_texture(texture_ref)) {
            size_t index = std::stoul(texture_ref.substr(1));
            resolved_path = extract_embedded_texture(index);
        } else {
            auto path_opt = resolve_texture_path(texture_ref);
            if (!path_opt) {
                std::cerr << "Could not resolve texture: " << texture_ref << std::endl;
                return INVALID_ASSET_ID;
            }
            resolved_path = *path_opt;
        }

        // Check if already registered
        if (auto existing = registry_.get_uuid_by_path(resolved_path)) {
            return *existing;
        }

        // For textures, we just register the original file
        return registry_.register_asset(resolved_path, AssetType::TEXTURE);
    }

    std::optional<std::filesystem::path> ModelImporter::resolve_texture_path(const std::string &texture_ref) const {
        const std::filesystem::path tex_filename = std::filesystem::path(texture_ref).filename();

        const std::vector search_paths = {
            source_dir_ / texture_ref,
            source_dir_ / tex_filename,
            source_dir_ / "textures" / tex_filename,
            source_dir_ / "Textures" / tex_filename,
            source_dir_ / "materials" / tex_filename,
            source_dir_.parent_path() / "textures" / tex_filename,
        };

        for (const auto &path: search_paths) {
            if (exists(path)) {
                return canonical(path);
            }
        }

        return std::nullopt;
    }

    bool ModelImporter::is_embedded_texture(const std::string &path) const {
        return !path.empty() && path[0] == '*';
    }

    std::filesystem::path ModelImporter::extract_embedded_texture(size_t index) {
        if (index >= ai_scene_->mNumTextures) {
            return {};
        }

        const aiTexture *tex = ai_scene_->mTextures[index];

        std::string extension = tex->achFormatHint;
        if (extension.empty()) {
            // Detect from magic bytes
            const auto *data = reinterpret_cast<const unsigned char *>(tex->pcData);
            if (data[0] == 0xFF && data[1] == 0xD8) extension = "jpg";
            else if (data[0] == 0x89 && data[1] == 0x50) extension = "png";
            else extension = "bin";
        }

        const std::string filename = make_unique_name(base_name_, "texture", index)
                                     + "." + extension;
        const auto filepath = output_dir_ / filename;

        std::ofstream file(filepath, std::ios::binary);
        if (!file) return {};

        // mHeight == 0 means compressed format, mWidth is byte count
        const size_t size = (tex->mHeight == 0)
                                ? tex->mWidth
                                : tex->mWidth * tex->mHeight * 4;

        file.write(reinterpret_cast<const char *>(tex->pcData), size);

        return filepath;
    }

    glm::mat4 ModelImporter::convert_matrix(const aiMatrix4x4 &m) {
        return glm::mat4(
            m.a1, m.b1, m.c1, m.d1,
            m.a2, m.b2, m.c2, m.d2,
            m.a3, m.b3, m.c3, m.d3,
            m.a4, m.b4, m.c4, m.d4
        );
    }

    bool ModelImporter::is_identity(const aiMatrix4x4 &m) {
        constexpr float epsilon = 0.0001f;
        return std::abs(m.a1 - 1.0f) < epsilon && std::abs(m.b2 - 1.0f) < epsilon
               && std::abs(m.c3 - 1.0f) < epsilon && std::abs(m.d4 - 1.0f) < epsilon
               && std::abs(m.a2) < epsilon && std::abs(m.a3) < epsilon && std::abs(m.a4) < epsilon
               && std::abs(m.b1) < epsilon && std::abs(m.b3) < epsilon && std::abs(m.b4) < epsilon
               && std::abs(m.c1) < epsilon && std::abs(m.c2) < epsilon && std::abs(m.c4) < epsilon
               && std::abs(m.d1) < epsilon && std::abs(m.d2) < epsilon && std::abs(m.d3) < epsilon;
    }

    std::string ModelImporter::make_unique_name(const std::string &base, const std::string &suffix,
                                                size_t index) const {
        return base + "_" + suffix + "_" + std::to_string(index);
    }
} // hellfire
