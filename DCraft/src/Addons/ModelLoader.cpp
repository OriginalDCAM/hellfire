#include "DCraft/Addons/ModelLoader.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "DCraft/Components/RenderableComponent.h"
#include "DCraft/Structs/Entity.h"

namespace fs = std::filesystem;

namespace DCraft::Addons {
    // Static member definitions
    std::unordered_map<std::string, std::shared_ptr<Mesh> > ModelLoader::mesh_cache;
    std::unordered_map<std::string, std::shared_ptr<Material> > ModelLoader::material_cache;

    std::unique_ptr<Entity> ModelLoader::load_model(const std::filesystem::path &filepath, unsigned int import_flags) {
        auto start_time = std::chrono::high_resolution_clock::now();
        std::cout << "Loading model: " << filepath << std::endl;

        Assimp::Importer importer;

        // Use provided flags or default to runtime
        if (import_flags == 0) {
            import_flags = ImportFlags::RUNTIME;
        }

        // Parse model file
        const aiScene *scene = importer.ReadFile(filepath.string(), import_flags);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ModelLoader: Cannot load " << filepath << " - "
                    << importer.GetErrorString() << std::endl;
            return nullptr;
        }

        // Debug information
        debug_scene_info(scene, filepath.string());

        // Pre-process materials for caching
        preprocess_materials(scene, filepath.string());

        // Process the scene
        std::unique_ptr<Entity> imported_model = process_node(scene->mRootNode, scene, filepath.string());

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        std::cout << "Model loaded in: " << duration.count() << "ms" << std::endl;

        return imported_model;
    }

    void ModelLoader::debug_scene_info(const aiScene *scene, const std::string &filepath) {
        std::cout << "=== Scene Debug Info for " << fs::path(filepath).filename() << " ===" << std::endl;
        std::cout << "Materials: " << scene->mNumMaterials << std::endl;
        std::cout << "Meshes: " << scene->mNumMeshes << std::endl;
        std::cout << "Embedded Textures: " << scene->mNumTextures << std::endl;

        // Print embedded texture info
        for (unsigned int i = 0; i < scene->mNumTextures; i++) {
            const aiTexture *tex = scene->mTextures[i];
            std::cout << "  Embedded texture " << i << ": "
                    << tex->mWidth << "x" << tex->mHeight
                    << " format: " << tex->achFormatHint << std::endl;
        }

        std::cout << "=============================================" << std::endl;
    }

    void ModelLoader::preprocess_materials(const aiScene *scene, const std::string &filepath) {
        std::cout << "Preprocessing " << scene->mNumMaterials << " materials..." << std::endl;

        for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
            const aiMaterial *ai_material = scene->mMaterials[i];

            // Create material cache key
            std::string cache_key = create_material_key(ai_material, filepath, i);

            // Skip if already cached
            if (material_cache.find(cache_key) != material_cache.end()) {
                continue;
            }

            // Create and cache material
            auto material = create_material(ai_material, scene, filepath);
            material_cache[cache_key] = std::shared_ptr<Material>(material.release());
        }
    }

    // This method converts assimp matrix struct to glm matrix struct
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
        glm::vec3 translation, scale, skew;
        glm::quat rotation;
        glm::vec4 perspective;
        glm::decompose(transform, scale, rotation, translation, skew, perspective);

        entity->transform()->set_position(translation);
        entity->transform()->set_scale(scale);
        entity->transform()->set_rotation(rotation);

        // Process meshes
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            unsigned int mesh_index = node->mMeshes[i];
            aiMesh *mesh = scene->mMeshes[mesh_index];

            std::shared_ptr<Mesh> processed_mesh = process_mesh(mesh, scene, filepath);

            if (node->mNumMeshes == 1) {
                auto renderable_component = entity->add_component<RenderableComponent>();
                renderable_component->set_mesh(processed_mesh);
            } else {
                // Multiple meshes: create child entity for each mesh
                std::string mesh_name = mesh->mName.length > 0 ? mesh->mName.C_Str() : ("Mesh_" + std::to_string(i));
                std::unique_ptr<Entity> mesh_entity = std::make_unique<Entity>(mesh_name);
                const auto renderable_component = mesh_entity->add_component<RenderableComponent>();
                renderable_component->set_mesh(processed_mesh);
                entity->add(mesh_entity.release());
            }
        }

        for (unsigned int child_index = 0; child_index < node->mNumChildren; child_index++) {
            std::unique_ptr<Entity> child = process_node(node->mChildren[child_index], scene, filepath);
            entity->add(child.release());
        }

        return entity;
    }

    void ModelLoader::process_mesh_vertices(aiMesh *mesh, std::vector<Vertex> &vertices,
                                            std::vector<unsigned int> &indices) {
        vertices.reserve(mesh->mNumVertices);
        indices.reserve(mesh->mNumFaces * 3);

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex &v = vertices.emplace_back();
            // Position
            const auto &pos = mesh->mVertices[i];
            v.position = glm::vec3(pos.x, pos.y, pos.z);

            // Normal
            if (mesh->HasNormals()) {
                const auto &norm = mesh->mNormals[i];
                v.normal = glm::vec3(norm.x, norm.y, norm.z);
            } else {
                v.normal = glm::vec3(0.0f, 1.0f, 0.0f); // Default up
            }

            // Color
            if (mesh->HasVertexColors(0)) {
                const auto &col = mesh->mColors[0][i];
                v.color = glm::vec3(col.r, col.g, col.b);
            } else {
                v.color = glm::vec3(1.0f); // Default white
            }

            // Texture coordinates
            if (mesh->HasTextureCoords(0)) {
                const auto &tex = mesh->mTextureCoords[0][i];
                v.texCoords = glm::vec2(tex.x, tex.y);
            } else {
                v.texCoords = glm::vec2(0.0f);
            }
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            const aiFace &face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    std::string ModelLoader::create_mesh_key(aiMesh *mesh, const std::string &filepath) {
        return filepath + "_mesh_" + std::string(mesh->mName.C_Str()) + "_" + std::to_string(mesh->mMaterialIndex);
    }

    std::string ModelLoader::create_material_key(const aiMaterial *ai_material, const std::string &filepath, unsigned int material_index) {
        aiString material_name;
        ai_material->Get(AI_MATKEY_NAME, material_name);
        return filepath + "_mat_" + std::to_string(material_index) + "_" + 
               (material_name.length > 0 ? material_name.C_Str() : "unnamed");
    }

    std::shared_ptr<Mesh> ModelLoader::process_mesh(aiMesh *mesh, const aiScene *scene, const std::string &filepath) {
        // create a mesh key and try to find it in cache
        std::string mesh_key = create_mesh_key(mesh, filepath);
        auto cached = mesh_cache.find(mesh_key);
        if (cached != mesh_cache.end()) {
            std::cout << "Using cached mesh: " << mesh_key << std::endl;
            return mesh_cache[mesh_key];
        }

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        process_mesh_vertices(mesh, vertices, indices);

        auto processed_mesh = std::make_shared<Mesh>(vertices, indices);

        if (mesh->mMaterialIndex >= 0) {
            const aiMaterial *ai_material = scene->mMaterials[mesh->mMaterialIndex];
            std::string material_key = create_material_key(ai_material, filepath, mesh->mMaterialIndex);
            
            auto cached_material = material_cache.find(material_key);
            if (cached_material != material_cache.end()) {
                // Use cached material
                processed_mesh->set_material(cached_material->second.get());
            } else {
                // Fallback to creating material 
                auto material = create_material(ai_material, scene, filepath);
                processed_mesh->set_material(material.release());
            }
        }

        mesh_cache[mesh_key] = processed_mesh;
        return processed_mesh;
    }


    std::unique_ptr<Material> ModelLoader::create_material(const aiMaterial *ai_material, const aiScene *scene, const std::string &filepath
    ) {
        // Get material name
        aiString material_name;
        ai_material->Get(AI_MATKEY_NAME, material_name);
        std::string name = material_name.length > 0 ? material_name.C_Str() : "ImportedMaterial";

        // Create Lambert Material 
        auto material = MaterialBuilder::create_lambert(name);

        // Load ALL properties
        load_essential_material_properties(ai_material, *material);

        // Load textures
        load_material_textures(ai_material, *material, scene, filepath);

        return material;
    }

    void ModelLoader::load_essential_material_properties(const aiMaterial *ai_material, Material &material) {
        aiColor3D color;
        float value;

        if (ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            material.set_diffuse_color(glm::vec3(color.r, color.g, color.b));
            material.set_property("diffuse", glm::vec3(color.r, color.g, color.b));
            material.set_property("albedo", glm::vec3(color.r, color.g, color.b));
        }

        if (ai_material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
            material.set_ambient_color(glm::vec3(color.r, color.g, color.b));
            material.set_property("ambient", glm::vec3(color.r, color.g, color.b));
        }

        if (ai_material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
            material.set_specular_color(glm::vec3(color.r, color.g, color.b));
            material.set_property("specular", glm::vec3(color.r, color.g, color.b));
        }

        if (ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
            material.set_property("emissive", glm::vec3(color.r, color.g, color.b));
        }

        if (ai_material->Get(AI_MATKEY_OPACITY, value) == AI_SUCCESS) {
            material.set_transparency(value);
            material.set_property("opacity", value);
            material.set_property("alpha", value);
        }

        if (ai_material->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS) {
            material.set_shininess(value);
            material.set_property("shininess", value);
        }

        if (ai_material->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS) {
            material.set_property("metallic", value);
        }

        if (ai_material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS) {
            material.set_property("roughness", value);
        }
    }

    std::vector<std::string> ModelLoader::get_texture_search_paths(const std::string& filepath) {
        std::filesystem::path model_dir = std::filesystem::path(filepath).parent_path();
        
        return {
            model_dir.string() + "/textures/",
            model_dir.string() + "/Textures/",
            model_dir.string() + "/texture/",
            model_dir.string() + "/materials/",
            model_dir.string() + "/source/",
            model_dir.string() + "/../textures/",
            model_dir.string() + "/",
            "assets/models/",
            "assets/textures/"
        };
    }

        void ModelLoader::load_material_textures(const aiMaterial *ai_material, Material &material, const aiScene *scene, const std::string &filepath) {
        // Pre-compute search paths once
        static thread_local std::vector<std::string> search_paths;
        if (search_paths.empty()) {
            search_paths = get_texture_search_paths(filepath);
        }

        auto load_texture_fast = [&](aiTextureType ai_type, TextureType dcr_type, const std::string& property_name) {
            if (ai_material->GetTextureCount(ai_type) == 0) return;
            
            aiString texture_path;
            if (ai_material->GetTexture(ai_type, 0, &texture_path) != AI_SUCCESS) return;
            
            std::string path_str = texture_path.C_Str();
            std::cout << "Processing " << property_name << " texture: " << path_str << std::endl;
            
            // Try embedded texture first
            if (try_load_embedded_texture(path_str, scene, dcr_type, material, property_name)) {
                return;
            }
            
            // Try external texture
            if (try_load_external_texture(path_str, filepath, dcr_type, material, property_name)) {
                return;
            }
            
            std::cerr << "Failed to load texture: " << path_str << std::endl;
        };

        // Load most common texture types
        load_texture_fast(aiTextureType_DIFFUSE, TextureType::DIFFUSE, "uDiffuseTexture");
        load_texture_fast(aiTextureType_NORMALS, TextureType::NORMAL, "uNormalTexture");
        load_texture_fast(aiTextureType_SPECULAR, TextureType::SPECULAR, "uSpecularTexture");
        load_texture_fast(aiTextureType_METALNESS, TextureType::METALNESS, "uMetallicTexture");
        load_texture_fast(aiTextureType_DIFFUSE_ROUGHNESS, TextureType::ROUGHNESS, "uRoughnessTexture");
        load_texture_fast(aiTextureType_AMBIENT_OCCLUSION, TextureType::AMBIENT_OCCLUSION, "uAoTexture");
        load_texture_fast(aiTextureType_EMISSIVE, TextureType::EMISSIVE, "uEmissiveTexture");
    }

        bool ModelLoader::try_load_embedded_texture(const std::string& path_str, const aiScene* scene, TextureType dcr_type, Material& material, const std::string& property_name) {
        if (path_str.empty() || path_str[0] != '*') {
            return false;
        }

        std::cout << "Found embedded texture reference: " << path_str << std::endl;

        try {
            int texture_index = std::stoi(path_str.substr(1));
            
            if (!scene || texture_index >= static_cast<int>(scene->mNumTextures)) {
                std::cerr << "Embedded texture index " << texture_index << " is out of range! Scene has " 
                         << (scene ? scene->mNumTextures : 0) << " embedded textures" << std::endl;
                return false;
            }

            const aiTexture* embedded_texture = scene->mTextures[texture_index];
            
            std::cout << "Processing embedded texture " << texture_index << std::endl;
            std::cout << "Format hint: '" << embedded_texture->achFormatHint << "'" << std::endl;
            std::cout << "Dimensions: " << embedded_texture->mWidth << "x" << embedded_texture->mHeight << std::endl;

            // Handle compressed embedded textures (PNG, JPG, etc.)
            if (embedded_texture->mHeight == 0) {
                // Determine file extension
                std::string extension = embedded_texture->achFormatHint;
                if (extension.empty() || extension.length() > 4) {
                    // Try to detect format from magic bytes
                    const unsigned char* data = reinterpret_cast<const unsigned char*>(embedded_texture->pcData);
                    if (data && embedded_texture->mWidth >= 2) {
                        if (data[0] == 0xFF && data[1] == 0xD8) extension = "jpg";
                        else if (data[0] == 0x89 && data[1] == 0x50) extension = "png";
                        else if (data[0] == 'B' && data[1] == 'M') extension = "bmp";
                        else extension = "bin";
                    } else {
                        extension = "bin";
                    }
                }

                // Save embedded texture to temp file and load it
                std::string temp_filename = "temp_texture_" + std::to_string(texture_index) + "." + extension;

                std::ofstream temp_file(temp_filename, std::ios::binary);
                if (!temp_file) {
                    std::cerr << "Failed to create temp file: " << temp_filename << std::endl;
                    return false;
                }
                
                temp_file.write(reinterpret_cast<const char*>(embedded_texture->pcData), embedded_texture->mWidth);
                temp_file.close();

                std::cout << "Saved embedded texture to: " << temp_filename << " (" << embedded_texture->mWidth << " bytes)" << std::endl;

                try {
                    auto texture = new Texture(temp_filename, dcr_type);
                    if (texture && texture->is_valid()) {
                        material.add_texture(*texture, property_name, 0);
                        std::cout << "Successfully loaded embedded texture: " << property_name << std::endl;
                        std::filesystem::remove(temp_filename);
                        return true;
                    } else {
                        std::cout << "Failed to create valid texture from embedded data" << std::endl;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Exception loading embedded texture: " << e.what() << std::endl;
                }
                
                std::filesystem::remove(temp_filename);
                return false;
            }
            // Handle uncompressed embedded textures (raw RGBA data)
            else {
                std::cout << "Uncompressed embedded texture found - needs direct GPU upload implementation" << std::endl;
                // TODO: Implement direct GPU upload from RGBA data
                // The data is in embedded_texture->pcData as RGBA pixels
                return false;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error processing embedded texture: " << e.what() << std::endl;
            return false;
        }
    }

        bool ModelLoader::try_load_external_texture(const std::string& path_str, const std::string& filepath, TextureType dcr_type, Material& material, const std::string& property_name) {
        std::string texture_filename = std::filesystem::path(path_str).filename().string();
        std::filesystem::path model_dir = std::filesystem::path(filepath).parent_path();

        // Try different path combinations
        std::vector<std::string> possible_paths = {
            path_str,  // Original path
            model_dir.string() + "/" + path_str,  // Relative to model file
            model_dir.string() + "/textures/" + texture_filename,  // textures subfolder
            model_dir.string() + "/Textures/" + texture_filename,  // Textures subfolder
            model_dir.string() + "/texture/" + texture_filename,   // texture subfolder
            model_dir.string() + "/materials/" + texture_filename, // materials folder
            model_dir.string() + "/source/" + texture_filename,    // source folder
            model_dir.string() + "/../textures/" + texture_filename, // Parent directory textures
            model_dir.string() + "/" + texture_filename,  // Same directory as model
            "assets/models/" + texture_filename,  // Common assets structure
            "assets/textures/" + texture_filename
        };

        for (const auto& test_path : possible_paths) {
            if (std::filesystem::exists(test_path)) {
                try {
                    if (const auto texture = new Texture(test_path, dcr_type); texture->is_valid()) {
                        material.add_texture(*texture, property_name, 0);
                        std::cout << "Loaded external texture: " << property_name << " from " << test_path << std::endl;
                        return true;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Failed to load texture from " << test_path << ": " << e.what() << std::endl;
                    continue;
                }
            }
        }

        return false;
    }



        std::string ModelLoader::capitalize_first(const std::string &str) {
        if (str.empty()) return str;
        std::string result = str;
        result[0] = std::toupper(result[0]);
        return result;
    }
    

    void ModelLoader::clear_cache() {
        mesh_cache.clear();
        material_cache.clear();
        std::cout << "ModelLoader caches cleared" << std::endl;
    }

    void ModelLoader::print_cache_stats() {
        std::cout << "=== ModelLoader Cache Statistics ===" << std::endl;
        std::cout << "Cached meshes: " << mesh_cache.size() << std::endl;
        std::cout << "Cached materials: " << material_cache.size() << std::endl;
        std::cout << "====================================" << std::endl;
    }
}
