#include "DCraft/Addons/ModelLoader.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>  // Fixed: was .inl
#include <glm/gtx/matrix_decompose.hpp>
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "DCraft/Structs/Scene.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace DCraft::Addons {
    std::unordered_map<std::string, std::shared_ptr<Mesh>> ModelLoader::mesh_cache;
    std::vector<ModelLoader::MaterialMap> ModelLoader::textures_loaded;

    ImportedModel3D* ModelLoader::load(const std::string& filepath, Scene* target_scene, ShaderManager& shader_manager) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            filepath,
            aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return nullptr;
        }

        // Create default material using new system
        auto default_material = MaterialBuilder::create_lambert("ModelMaterial");
        default_material->set_ambient_color(glm::vec3(0.2f, 0.2f, 0.2f));
        default_material->set_diffuse_color(glm::vec3(0.8f, 0.8f, 0.8f));
        
        // Assign shader to default material
        uint32_t shader_id = shader_manager.get_shader_for_material(*default_material);
        default_material->set_compiled_shader_id(shader_id);

        std::string filename = filepath;
        size_t last_slash = filepath.find_last_of("/\\");
        if (last_slash != std::string::npos) {
            filename = filepath.substr(last_slash + 1);
        }

        ImportedModel3D* imported_shape = process_node(scene->mRootNode, scene, 
                                                      default_material.get(), shader_manager);
        imported_shape->set_name(filename);

        fs::path absolute_path = filepath;
        fs::path assets_dir = "assets";

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

        // Keep default material alive (you might want to manage this differently)
        default_material.release();

        return imported_shape;
    }

    glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from) {
        glm::mat4 to;
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    }

    ImportedModel3D* ModelLoader::process_node(aiNode* node, const aiScene* scene, 
                                              Material* default_material, ShaderManager& shader_manager) {
        ImportedModel3D* obj = new ImportedModel3D(node->mName.length > 0 ? node->mName.C_Str() : "Node");

        std::cout << "Node: " << node->mName.C_Str() << ", Meshes: " << node->mNumMeshes
                  << ", Children: " << node->mNumChildren << std::endl;

        aiMatrix4x4 transform = node->mTransformation;
        glm::mat4 glm_transform = aiMatrix4x4ToGlm(transform);

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
            aiMesh* mesh = scene->mMeshes[meshIndex];
            std::shared_ptr<Mesh> my_mesh = process_mesh(mesh, scene, default_material, shader_manager);

            if (node->mNumMeshes > 1) {
                Object3D* meshObj = new Object3D(mesh->mName.length > 0
                                                ? mesh->mName.C_Str()
                                                : "Mesh" + std::to_string(i));
                meshObj->set_mesh(my_mesh);
                obj->add(meshObj);
            } else {
                obj->set_mesh(my_mesh);
            }
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            Object3D* child = process_node(node->mChildren[i], scene, default_material, shader_manager);
            obj->add(child);
        }

        return obj;
    }

    std::shared_ptr<Mesh> ModelLoader::process_mesh(aiMesh* mesh, const aiScene* scene, 
                                                   Material* default_material, ShaderManager& shader_manager) {
        size_t vertex_hash = 0;
        size_t sample_size = std::min(static_cast<size_t>(10), static_cast<size_t>(mesh->mNumVertices));

        for (size_t i = 0; i < sample_size; i++) {
            aiVector3D pos = mesh->mVertices[i];
            vertex_hash = vertex_hash ^ (std::hash<float>()(pos.x) +
                                        (std::hash<float>()(pos.y) << 1) +
                                        (std::hash<float>()(pos.z) << 2));
        }

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

        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        std::shared_ptr<Mesh> myMesh = std::make_shared<Mesh>(vertices, indices);
        myMesh->set_material(default_material);

        // Enhanced material processing
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* ai_material = scene->mMaterials[mesh->mMaterialIndex];

            try {
                std::unique_ptr<Material> material = create_material_from_assimp(ai_material, shader_manager);
                myMesh->set_material(material.release()); // Transfer ownership
            } catch (const std::exception& e) {
                std::cerr << "Failed to create material: " << e.what() << std::endl;
                // Keep default material
            }
        }

        mesh_cache[meshKey] = myMesh;
        return myMesh;
    }

    std::unique_ptr<Material> ModelLoader::create_material_from_assimp(aiMaterial* ai_material, ShaderManager& shader_manager) {
        aiString material_name;
        ai_material->Get(AI_MATKEY_NAME, material_name);
        std::string name = material_name.length > 0 ? material_name.C_Str() : "ImportedMaterial";

        MaterialType mat_type = determine_material_type(ai_material);

        std::unique_ptr<Material> material;

        switch (mat_type) {
            case MaterialType::PBR:
                material = MaterialBuilder::create_pbr(name);
                load_pbr_properties(ai_material, *material);
                break;
            case MaterialType::PHONG:
                material = MaterialBuilder::create_phong(name);
                load_phong_properties(ai_material, *material);
                break;
            case MaterialType::LAMBERT:
            default:
                material = MaterialBuilder::create_lambert(name);
                load_lambert_properties(ai_material, *material);
                break;
        }

        load_material_textures_enhanced(ai_material, *material);

        uint32_t shader_id = shader_manager.get_shader_for_material(*material);
        material->set_compiled_shader_id(shader_id);

        return material;
    }

    // Rest of the implementation methods stay the same as provided earlier...
    ModelLoader::MaterialType ModelLoader::determine_material_type(aiMaterial* ai_material) {
        float metallic = 0.0f;
        float roughness = 0.0f;
        
        if (ai_material->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS ||
            ai_material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS ||
            ai_material->GetTextureCount(aiTextureType_METALNESS) > 0 ||
            ai_material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0) {
            return MaterialType::PBR;
        }
        
        float shininess = 0.0f;
        aiColor3D specular;
        
        if (ai_material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS ||
            ai_material->Get(AI_MATKEY_COLOR_SPECULAR, specular) == AI_SUCCESS ||
            ai_material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
            return MaterialType::PHONG;
        }
        
        return MaterialType::LAMBERT;
    }

    void ModelLoader::load_lambert_properties(aiMaterial* ai_material, Material& material) {
        aiColor3D diffuse(0.8f, 0.8f, 0.8f);
        if (ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS) {
            material.set_diffuse_color(glm::vec3(diffuse.r, diffuse.g, diffuse.b));
        }
        
        aiColor3D ambient(0.2f, 0.2f, 0.2f);
        if (ai_material->Get(AI_MATKEY_COLOR_AMBIENT, ambient) == AI_SUCCESS) {
            material.set_ambient_color(glm::vec3(ambient.r, ambient.g, ambient.b));
        }
        
        float opacity = 1.0f;
        if (ai_material->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS) {
            material.set_property("opacity", opacity);
        }
    }

    void ModelLoader::load_phong_properties(aiMaterial* ai_material, Material& material) {
        load_lambert_properties(ai_material, material);
        
        aiColor3D specular(0.5f, 0.5f, 0.5f);
        if (ai_material->Get(AI_MATKEY_COLOR_SPECULAR, specular) == AI_SUCCESS) {
            material.set_specular_color(glm::vec3(specular.r, specular.g, specular.b));
        }
        
        float shininess = 32.0f;
        if (ai_material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
            material.set_shininess(shininess);
        }
        
        float specular_strength = 1.0f;
        if (ai_material->Get(AI_MATKEY_SHININESS_STRENGTH, specular_strength) == AI_SUCCESS) {
            material.set_property("specularStrength", specular_strength);
        }
    }

    void ModelLoader::load_pbr_properties(aiMaterial* ai_material, Material& material) {
        aiColor3D base_color(0.8f, 0.8f, 0.8f);
        if (ai_material->Get(AI_MATKEY_BASE_COLOR, base_color) == AI_SUCCESS) {
            material.set_diffuse_color(glm::vec3(base_color.r, base_color.g, base_color.b));
        } else {
            aiColor3D diffuse(0.8f, 0.8f, 0.8f);
            if (ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS) {
                material.set_diffuse_color(glm::vec3(diffuse.r, diffuse.g, diffuse.b));
            }
        }
        
        float metallic = 0.0f;
        if (ai_material->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS) {
            material.set_property("metallic", metallic);
        }
        
        float roughness = 0.5f;
        if (ai_material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS) {
            material.set_property("roughness", roughness);
        }
        
        material.set_property("ao", 1.0f);
        
        aiColor3D emissive(0.0f, 0.0f, 0.0f);
        if (ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive) == AI_SUCCESS) {
            material.set_property("emissiveColor", glm::vec3(emissive.r, emissive.g, emissive.b));
        }
    }

    void ModelLoader::load_material_textures_enhanced(aiMaterial* ai_material, Material& material) {
        load_texture_if_exists(ai_material, aiTextureType_DIFFUSE, material, "diffuseTexture");
        load_texture_if_exists(ai_material, aiTextureType_NORMALS, material, "normalTexture");
        load_texture_if_exists(ai_material, aiTextureType_SPECULAR, material, "specularTexture");
        load_texture_if_exists(ai_material, aiTextureType_METALNESS, material, "metallicTexture");
        load_texture_if_exists(ai_material, aiTextureType_DIFFUSE_ROUGHNESS, material, "roughnessTexture");
        load_texture_if_exists(ai_material, aiTextureType_AMBIENT_OCCLUSION, material, "aoTexture");
        load_texture_if_exists(ai_material, aiTextureType_EMISSIVE, material, "emissionTexture");
    }

    void ModelLoader::load_texture_if_exists(aiMaterial* ai_material, aiTextureType type, 
                                            Material& material, const std::string& property_name) {
        if (ai_material->GetTextureCount(type) > 0) {
            aiString texture_path;
            if (ai_material->GetTexture(type, 0, &texture_path) == AI_SUCCESS) {
                std::string path = "models/" + std::string(texture_path.C_Str());
                
                Texture* texture = get_or_load_texture(path);
                if (texture) {
                    material.set_property(property_name, texture);
                    std::string use_flag = "use" + capitalize_first(property_name);
                    material.set_property(use_flag, true);
                }
            }
        }
    }

    Texture* ModelLoader::get_or_load_texture(const std::string& path) {
        static std::unordered_map<std::string, std::unique_ptr<Texture>> texture_cache;
        
        if (texture_cache.find(path) != texture_cache.end()) {
            return texture_cache[path].get();
        }
        
        try {
            auto texture = std::make_unique<Texture>(path, TextureType::DIFFUSE);
            Texture* raw_ptr = texture.get();
            texture_cache[path] = std::move(texture);
            return raw_ptr;
        } catch (const std::exception& e) {
            std::cerr << "Failed to load texture: " << path << " - " << e.what() << std::endl;
            return nullptr;
        }
    }

    std::string ModelLoader::capitalize_first(const std::string& str) {
        if (str.empty()) return str;
        std::string result = str;
        result[0] = std::toupper(result[0]);
        return result;
    }

    // Keep legacy texture loading for backwards compatibility
    std::vector<ModelLoader::MaterialMap> ModelLoader::load_material_textures(
        aiMaterial* material, aiTextureType type, std::string typeName) {
        std::vector<MaterialMap> textures;
        for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
            aiString str;
            material->GetTexture(type, i, &str);
            
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
                texture.path = "models/" + path;
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }
}