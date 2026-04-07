//
// Created by denzel on 09/04/2025.
//
#include "DCraft/Addons/ModelLoader.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.inl>
#include <glm/gtx/matrix_decompose.hpp>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "DCraft/Addons/ImportedModel3D.h"
#include "DCraft/Graphics/Materials/LambertMaterial.h"
#include "DCraft/Graphics/Primitives/Shape3D.h"
#include "DCraft/Structs/Scene.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace DCraft::Addons {
    std::unordered_map<std::string, Mesh *> ModelLoader::mesh_cache;
    std::vector<ModelLoader::MaterialMap> ModelLoader::textures_loaded;

    ImportedModel3D *ModelLoader::load(const std::string &filepath, Scene *target_scene) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(
            filepath,
            aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return nullptr;
        }

        // Default material for model
        LambertMaterial *defaultMaterial = new LambertMaterial("ModelMaterial");
        defaultMaterial->set_ambient_color(glm::vec3(0.2f, 0.2f, 0.2f));
        defaultMaterial->set_diffuse_color(glm::vec3(0.8f, 0.8, 0.8f));

        std::string filename = filepath;
        size_t last_slash = filepath.find_last_of("/\\");
        if (last_slash != std::string::npos) {
            filename = filepath.substr(last_slash + 1);
        }

        ImportedModel3D *imported_shape = process_node(scene->mRootNode, scene, defaultMaterial);
        imported_shape->set_name(filename);

        fs::path absolute_path = filepath;
        fs::path assets_dir = "assets";

        // Check if assets_dir is part of the path
        std::string path_str = absolute_path.string();
        size_t pos = path_str.find(assets_dir.string());

        if (pos != std::string::npos) {
            std::string relative_path = path_str.substr(pos);
            imported_shape->set_file_path(relative_path);
        } else {
            imported_shape->set_file_path(path_str);
        }

        if (target_scene) {
            target_scene->add(imported_shape);
        }

        return imported_shape;
    }

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


    ImportedModel3D *ModelLoader::process_node(aiNode *node, const aiScene *scene, Material *default_material) {
        ImportedModel3D *obj = new ImportedModel3D(node->mName.length > 0 ? node->mName.C_Str() : "Node");

        std::cout << "Node: " << node->mName.C_Str() << ", Meshes: " << node->mNumMeshes
                << ", Children: " << node->mNumChildren << std::endl;

        aiMatrix4x4 transform = node->mTransformation;
        glm::mat4 glm_transform = aiMatrix4x4ToGlm(transform);

        // Extract position, rotation, scale
        glm::vec3 translation, scale;
        glm::quat rotation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(glm_transform, scale, rotation, translation, skew, perspective);

        obj->set_position(translation);
        obj->set_scale(scale);
        obj->set_rotation_quaternion(rotation);

        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            uint32_t meshIndex = node->mMeshes[i];
            aiMesh *mesh = scene->mMeshes[meshIndex];
            Mesh *my_mesh = process_mesh(mesh, scene, default_material);

            if (node->mNumMeshes > 1) {
                Object3D *meshObj = new Object3D(mesh->mName.length > 0
                                                     ? mesh->mName.C_Str()
                                                     : "Mesh" + std::to_string(i));
                meshObj->set_mesh(my_mesh);
                obj->add(meshObj);
            } else {
                obj->set_mesh(my_mesh);
            }
        }

        // Process child nodes
        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            Object3D *child = process_node(node->mChildren[i], scene, default_material);
            obj->add(child);
        }

        return obj;
    }

    Mesh *ModelLoader::process_mesh(aiMesh *mesh, const aiScene *scene, Material *default_material) {
        size_t vertex_hash = 0;
        size_t sample_size = std::min(size_t(10), size_t(mesh->mNumVertices)); // Sample up to 10 vertices

        for (size_t i = 0; i < sample_size; i++) {
            // Hash the position of each sampled vertex
            aiVector3D pos = mesh->mVertices[i];
            vertex_hash = vertex_hash ^ (std::hash<float>()(pos.x) +
                                         (std::hash<float>()(pos.y) << 1) +
                                         (std::hash<float>()(pos.z) << 2));
        }

        // Create a unique mesh key that includes the mesh name, material index, vertex/face counts, and geometry hash
        std::string meshName = mesh->mName.length > 0 ? mesh->mName.C_Str() : "unnamed";
        std::string meshKey = meshName + "_" +
                              std::to_string(mesh->mMaterialIndex) + "_" +
                              std::to_string(mesh->mNumVertices) + "_" +
                              std::to_string(mesh->mNumFaces) + "_" +
                              std::to_string(vertex_hash);

        if (mesh_cache.find(meshKey) != mesh_cache.end()) {
            std::cout << "Using cached mesh: " << meshKey << std::endl;
            return mesh_cache[meshKey];
        }

        std::cout << "Creating new mesh: " << meshKey << std::endl;

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
            Vertex v;

            // Position
            v.position.x = mesh->mVertices[i].x;
            v.position.y = mesh->mVertices[i].y;
            v.position.z = mesh->mVertices[i].z;

            if (mesh->HasNormals()) {
                v.normal.x = mesh->mNormals[i].x;
                v.normal.y = mesh->mNormals[i].y;
                v.normal.z = mesh->mNormals[i].z;
            }

            if (mesh->HasTextureCoords(0)) {
                v.texCoords.x = mesh->mTextureCoords[0][i].x;
                v.texCoords.y = mesh->mTextureCoords[0][i].y;
            } else {
                v.texCoords = glm::vec2(0.0f);
            }

            v.color = glm::vec3(1.0f);

            vertices.push_back(v);
        }

        // Process indices
        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Create and process the mesh as before
        Mesh *myMesh = new Mesh(vertices, indices);
        myMesh->set_material(default_material);

        // Process materials if available
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

            LambertMaterial *lambert_material = new LambertMaterial(material->GetName().C_Str());

            // Setup material properties
            aiColor3D diffuse(1.0f, 1.0f, 1.0f);
            if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS) {
                lambert_material->set_diffuse_color(glm::vec3(diffuse.r, diffuse.g, diffuse.b));
            }

            aiColor3D ambient(0.2, 0.2, 0.2);
            if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambient) == AI_SUCCESS) {
                lambert_material->set_ambient_color(glm::vec3(ambient.r, ambient.g, ambient.b));
            }

            // Load textures if exist
            std::vector<MaterialMap> diffuseMaps = load_material_textures(
                material, aiTextureType_DIFFUSE, "texture_diffuse");
            if (!diffuseMaps.empty()) {
                lambert_material->set_texture(diffuseMaps[0].path, TextureType::DIFFUSE);
            }

            std::vector<MaterialMap> specularMaps = load_material_textures(
                material, aiTextureType_SPECULAR, "texture_specular");
            if (!specularMaps.empty()) {
                lambert_material->set_texture(specularMaps[0].path, TextureType::SPECULAR);
            }

            myMesh->set_material(lambert_material);
        }

        // Cache the mesh
        mesh_cache[meshKey] = myMesh;
        return myMesh;
    }

    std::vector<ModelLoader::MaterialMap> ModelLoader::load_material_textures(
        aiMaterial *material, aiTextureType type, std::string typeName) {
        std::vector<MaterialMap> textures;

        for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
            aiString str;
            material->GetTexture(type, i, &str);

            // Check if texture was loaded before
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++) {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }

            if (!skip) {
                MaterialMap texture;
                texture.type = typeName;

                std::string path = str.C_Str();
                texture.path = "assets/models/" + path;

                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }

        return textures;
    }
}
