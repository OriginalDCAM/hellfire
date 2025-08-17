#include "DCraft/Addons/ModelLoader.h"
#include <filesystem>
#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "DCraft/Components/RenderableComponent.h"
#include "DCraft/Structs/Entity.h"

namespace fs = std::filesystem;

namespace DCraft::Addons {
    std::unique_ptr<Entity> ModelLoader::load_model(const std::filesystem::path &filepath) {
        Assimp::Importer importer;

        // Parse model file
        const aiScene *scene = importer.ReadFile(
            filepath.string(),
            aiProcess_Triangulate | aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ModelLoader: Cannot load " << filepath << " - "
                    << importer.GetErrorString() << std::endl;
            return nullptr; // Not a supported 3D model
        }

        std::unique_ptr<Entity> imported_model = process_node(scene->mRootNode, scene, filepath.string());

        return imported_model;
    }

    // This method converts an Assimp Matrix which is row-major-matrix to glms matrix
    glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &from) {
        glm::mat4 to;
        to[0][0] = from.a1;
        to[1][0] = from.a2;
        to[2][0] = from.a3;
        to[3][0] = from.a4;
        to[0][1] = from.b1;
        to[1][1] = from.b2;
        to[2][1] = from.b3;
        to[3][1] = from.b4;
        to[0][2] = from.c1;
        to[1][2] = from.c2;
        to[2][2] = from.c3;
        to[3][2] = from.c4;
        to[0][3] = from.d1;
        to[1][3] = from.d2;
        to[2][3] = from.d3;
        to[3][3] = from.d4;
        return to;
    }


    std::unique_ptr<Entity> ModelLoader::process_node(aiNode *node, const aiScene *scene, const std::string &filepath) {
        std::unique_ptr<Entity> entity = std::make_unique<Entity>(node->mName.C_Str());

        glm::mat4 transform = aiMatrix4x4ToGlm(node->mTransformation);

        // Extract the transformation data from the transform matrix
        glm::vec3 translation, scale;
        glm::quat rotation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(transform, scale, rotation, translation, skew, perspective);

        entity->transform()->set_position(translation);
        entity->transform()->set_scale(scale);
        entity->transform()->set_rotation(rotation);

        for (size_t i = 0; i < node->mNumMeshes; i++) {
            size_t mesh_index = node->mMeshes[i];
            aiMesh *mesh = scene->mMeshes[mesh_index];

            std::shared_ptr<Mesh> processed_mesh = process_mesh(mesh, scene, filepath);

            if (node->mNumMeshes == 1) {
                RenderableComponent *renderable_component = entity->add_component<RenderableComponent>();
                renderable_component->set_mesh(processed_mesh);
            } else {
                // Multiple meshes: create child entity for each mesh
                std::string mesh_name = mesh->mName.length > 0 ? mesh->mName.C_Str() : ("Mesh_" + std::to_string(i));
                std::unique_ptr<Entity> mesh_entity = std::make_unique<Entity>(mesh_name);
                RenderableComponent *renderable_component = mesh_entity->add_component<RenderableComponent>();
                renderable_component->set_mesh(processed_mesh);
                entity->add(mesh_entity.release());
            }

        }

        for (size_t child_index = 0; child_index < node->mNumChildren; child_index++) {
            std::unique_ptr<Entity> child = process_node(node->mChildren[child_index], scene, filepath);
            entity->add(child.release());
        }

        return entity;
    }

    void ModelLoader::process_mesh_vertices(aiMesh *mesh, std::vector<Vertex> &vertices,
                                            std::vector<unsigned int> &indices) {
        for (size_t i = 0; i < mesh->mNumVertices; i++) {
            Vertex v;
            v.position.x = mesh->mVertices[i].x;
            v.position.y = mesh->mVertices[i].y;
            v.position.z = mesh->mVertices[i].z;

            v.color = glm::vec3(1.0f);

            if (mesh->HasNormals()) {
                v.normal.x = mesh->mNormals[i].x;
                v.normal.y = mesh->mNormals[i].y;
                v.normal.z = mesh->mNormals[i].z;
            }

            if (mesh->HasVertexColors(0)) {
                v.color.r = mesh->mColors[0][i].r;
                v.color.g = mesh->mColors[0][i].g;
                v.color.b = mesh->mColors[0][i].b;
            }

            if (mesh->HasTextureCoords(0)) {
                v.texCoords.x = mesh->mTextureCoords[0][i].x;
                v.texCoords.y = mesh->mTextureCoords[0][i].y;
            } else {
                v.texCoords = glm::vec2(0.0f);
            }

            vertices.push_back(v);
        }

        for (size_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (size_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    std::string create_mesh_key(aiMesh *mesh, const std::string &filepath) {
        return filepath + "_" +
               std::string(mesh->mName.C_Str()) + "_" +
               std::to_string(mesh->mMaterialIndex);
    }

    std::shared_ptr<Mesh> ModelLoader::process_mesh(aiMesh *mesh, const aiScene *scene, const std::string &filepath) {
        // create a mesh key and try to find it in cache
        std::string mesh_key = create_mesh_key(mesh, filepath);
        if (mesh_cache.find(mesh_key) != mesh_cache.end()) {
            std::cout << "Using cached mesh: " << mesh_key << std::endl;
            return mesh_cache[mesh_key];
        }

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        process_mesh_vertices(mesh, vertices, indices);

        auto processed_mesh = std::make_shared<Mesh>(vertices, indices);

        if (mesh->mMaterialIndex >= 0) {
            aiMaterial *ai_material = scene->mMaterials[mesh->mMaterialIndex];
            std::unique_ptr<Material> processed_material = create_material_from_assimp(ai_material);
            processed_mesh->set_material(processed_material.release());
        }

        mesh_cache[mesh_key] = processed_mesh;
        return processed_mesh;
    }

    std::unordered_map<std::string, std::shared_ptr<Mesh> > ModelLoader::mesh_cache;

    std::unique_ptr<Material> ModelLoader::create_material_from_assimp(aiMaterial *ai_material
    ) {
        // Get material name
        aiString material_name;
        ai_material->Get(AI_MATKEY_NAME, material_name);
        std::string name = material_name.length > 0 ? material_name.C_Str() : "ImportedMaterial";

        // Create Lambert Material 
        auto material = MaterialBuilder::create_lambert(name);

        // Load ALL properties
        load_material_properties(ai_material, *material);

        return material;
    }

    void ModelLoader::load_material_properties(const aiMaterial *ai_material, Material &material) {
        // === COLORS ===
        aiColor3D color;

        // Diffuse/Albedo (both names for compatibility)
        if (ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            material.set_property("diffuse", glm::vec3(color.r, color.g, color.b));
            material.set_property("albedo", glm::vec3(color.r, color.g, color.b)); // PBR name
            material.set_diffuse_color(glm::vec3(color.r, color.g, color.b)); // Convenience method
        }

        // Ambient
        if (ai_material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
            material.set_property("ambient", glm::vec3(color.r, color.g, color.b));
            material.set_ambient_color(glm::vec3(color.r, color.g, color.b));
        }

        // Specular  
        if (ai_material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
            material.set_property("specular", glm::vec3(color.r, color.g, color.b));
            material.set_specular_color(glm::vec3(color.r, color.g, color.b));
        }

        // Emissive
        if (ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
            material.set_property("emissive", glm::vec3(color.r, color.g, color.b));
        }

        // === SCALAR PROPERTIES ===
        float value;

        // Opacity/Alpha
        if (ai_material->Get(AI_MATKEY_OPACITY, value) == AI_SUCCESS) {
            material.set_property("opacity", value);
            material.set_property("alpha", value);
            material.set_transparency(value); // Uses convenience method
        }

        // Shininess
        if (ai_material->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS) {
            material.set_property("shininess", value);
            material.set_shininess(value);
        }

        // PBR Properties
        if (ai_material->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS) {
            material.set_property("metallic", value);
        }

        if (ai_material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS) {
            material.set_property("roughness", value);
        }

        // === TEXTURES ===
        load_material_textures(ai_material, material);
    }

    void ModelLoader::load_material_textures(const aiMaterial *ai_material, Material &material) {
        // Helper to load texture using your TextureCache
        auto load_texture = [&](aiTextureType ai_type, TextureType dcr_type, const std::string& property_name) {
            if (ai_material->GetTextureCount(ai_type) > 0) {
                aiString texture_path;
                if (ai_material->GetTexture(ai_type, 0, &texture_path) == AI_SUCCESS) {
                    const std::string full_path = "assets/models/" + std::string(texture_path.C_Str());
                
                    try {
                        // Use your TextureCache system
                        auto texture = TextureCache::load(full_path, dcr_type);
                        if (texture && texture->is_valid()) {
                            material.set_property(property_name, texture.get());
                            material.set_property("use" + capitalize_first(property_name), true);
                            std::cout << "Loaded texture: " << property_name << " from " << full_path << std::endl;
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Failed to load texture: " << full_path << " - " << e.what() << std::endl;
                    }
                }
            }
        };
    
        // Load all texture types
        load_texture(aiTextureType_DIFFUSE, TextureType::DIFFUSE, "diffuseTexture");
        load_texture(aiTextureType_NORMALS, TextureType::NORMAL, "normalTexture");
        load_texture(aiTextureType_SPECULAR, TextureType::SPECULAR, "specularTexture");
        load_texture(aiTextureType_METALNESS, TextureType::METALNESS, "metallicTexture");
        load_texture(aiTextureType_DIFFUSE_ROUGHNESS, TextureType::ROUGHNESS, "roughnessTexture");
        load_texture(aiTextureType_AMBIENT_OCCLUSION, TextureType::AMBIENT_OCCLUSION, "aoTexture");
        load_texture(aiTextureType_EMISSIVE, TextureType::EMISSIVE, "emissionTexture");
        
    }

    std::string ModelLoader::capitalize_first(const std::string &str) {
        if (str.empty()) return str;
        std::string result = str;
        result[0] = std::toupper(result[0]);
        return result;
    }
}
