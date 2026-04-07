//
// Created by denzel on 09/04/2025.
//
#include "DCraft/Addons/ModelLoader.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.inl>
#include <glm/gtx/matrix_decompose.hpp>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "DCraft/Graphics/Materials/PhongMaterial.h"
#include "DCraft/Graphics/Primitives/Shape3D.h"
#include "DCraft/Structs/Scene.h"

namespace DCraft::Addons {
    std::unordered_map<std::string, Mesh*> ModelLoader::mesh_cache;
    std::vector<ModelLoader::MaterialMap> ModelLoader::textures_loaded;
    
    Shape3D *ModelLoader::load(const std::string &filepath, Scene *target_scene) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(
            filepath,
            aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return nullptr;
        }

        // Default material for model
        PhongMaterial *defaultMaterial = new PhongMaterial("ModelMaterial");
        defaultMaterial->set_ambient_color(glm::vec3(0.2f, 0.2f, 0.2f));
        defaultMaterial->set_diffuse_color(glm::vec3(0.8f, 0.8, 0.8f));
        defaultMaterial->set_specular_color(glm::vec3(0.5f));
        defaultMaterial->set_shininess(32.0f);

        std::string filename = filepath;
        size_t last_slash = filepath.find_last_of("/\\");
        if (last_slash != std::string::npos) {
            filename = filepath.substr(last_slash + 1);
        }

        Object3D *rootObj = process_node(scene->mRootNode, scene, defaultMaterial);
        rootObj->set_name(filename);

        if (target_scene) {
            target_scene->add(rootObj);
        }

        return static_cast<Shape3D*>(rootObj);
    }

    glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &from) {
        glm::mat4 to;

        // Assimp matrices are row-major, glm matrices are column-major
        memcpy(glm::value_ptr(to), &from, sizeof(float) * 16);

        return glm::transpose(to);
    }


    Shape3D* ModelLoader::process_node(aiNode *node, const aiScene *scene, Material *default_material) {
        Shape3D *obj = new Shape3D(node->mName.length > 0 ? node->mName.C_Str() : "Node");

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

        // Todo convert the quat to euler angles for rotation

        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[i];
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
        std::string meshKey = std::to_string(mesh->mNumVertices) + "_" +
                              std::to_string(mesh->mNumFaces);

        if (mesh_cache.find(meshKey) != mesh_cache.end()) {
            return mesh_cache[meshKey];
        }

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

        // Proccess materials if available
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

            PhongMaterial *phong_material = new PhongMaterial(material->GetName().C_Str());

            // Setup material properties
            aiColor3D diffuse(1.0f, 1.0f, 1.0f);
            if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS) {
                phong_material->set_diffuse_color(glm::vec3(diffuse.r, diffuse.g, diffuse.b));
            }

            aiColor3D ambient(0.2, 0.2, 0.2);
            if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambient) == AI_SUCCESS) {
                phong_material->set_ambient_color(glm::vec3(ambient.r, ambient.g, ambient.b));
            }

            aiColor3D specular(0.5f, 0.5f, 0.5f);
            if (material->Get(AI_MATKEY_COLOR_SPECULAR, specular) == AI_SUCCESS) {
                phong_material->set_specular_color(glm::vec3(specular.r, specular.g, specular.b));
            }

            float shininess = 32.0f;
            if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
                phong_material->set_shininess(shininess);
            }

            // Load textures if exist
            std::vector<MaterialMap> diffuseMaps = load_material_textures(
                material, aiTextureType_DIFFUSE, "texture_diffuse");
            if (!diffuseMaps.empty()) {
                phong_material->set_texture(diffuseMaps[0].path, TextureType::DIFFUSE);
            }

            std::vector<MaterialMap> specularMaps = load_material_textures(
                material, aiTextureType_SPECULAR, "texture_specular");
            if (!specularMaps.empty()) {
                phong_material->set_texture(specularMaps[0].path, TextureType::SPECULAR);
            }

            myMesh->set_material(phong_material);
        }

        // Cache the mesh
        mesh_cache[meshKey] = myMesh;
        return myMesh;
    }

    std::vector<ModelLoader::MaterialMap> ModelLoader::load_material_textures(aiMaterial *material, aiTextureType type, std::string typeName) {
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

                // Extract the texture path
                std::string path = str.C_Str();
                // Make the path relative to your assets directory
                texture.path = "assets/models/" + path;

                textures.push_back(texture);
                textures_loaded.push_back(texture); // Add to loaded textures
            }
        }

        return textures;
    }
}
