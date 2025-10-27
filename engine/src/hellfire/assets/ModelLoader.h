#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "hellfire/graphics/Vertex.h"
#include "hellfire/graphics/material/Material.h"


namespace hellfire {
    class Mesh;
    using EntityID = uint32_t;
    class Scene;
}

namespace hellfire::Addons {
    class ModelLoader {
    public:
        // Updated method signatures to include ShaderManager
        static EntityID load_model(Scene* scene, const std::filesystem::path &filepath, unsigned int import_flags = 0);

        static void clear_cache();

        static void print_cache_stats();

        // Import flag presets
        struct ImportFlags {
            // For editor preview - preserve hierarchy, minimal processing
            static constexpr unsigned int PREVIEW =
                    aiProcess_Triangulate |
                    aiProcess_GenSmoothNormals |  
                    aiProcess_CalcTangentSpace |
                    aiProcess_JoinIdenticalVertices |
                    aiProcess_ValidateDataStructure;

            // For runtime use - preserve hierarchy but optimize vertices
            static constexpr unsigned int RUNTIME =
                    aiProcess_Triangulate |
                    aiProcess_GenSmoothNormals |
                    aiProcess_CalcTangentSpace |
                    aiProcess_JoinIdenticalVertices |
                    aiProcess_ImproveCacheLocality |
                    aiProcess_RemoveRedundantMaterials |
                    aiProcess_SortByPType |
                    aiProcess_FindInvalidData |
                    aiProcess_ValidateDataStructure;

            // For high quality/production builds
            static constexpr unsigned int HIGH_QUALITY =
                    RUNTIME |
                    aiProcess_GenUVCoords |
                    aiProcess_TransformUVCoords |
                    aiProcess_FindInstances |
                    aiProcess_OptimizeMeshes |  
                    aiProcess_Debone;
            
            // For baked/optimized static meshes 
            static constexpr unsigned int OPTIMIZED =
                    RUNTIME |
                    aiProcess_OptimizeMeshes |     // Merge meshes
                    aiProcess_OptimizeGraph |      // Flatten hierarchy
                    aiProcess_PreTransformVertices; // Bake transforms
        };

    private:
        enum class MaterialType {
            LAMBERT,
            PHONG,
            PBR
        };



        // Node processing
        static EntityID process_node(Scene* scene,aiNode *node, const aiScene *ai_scene, const std::string &filepath,EntityID parent_id = 0);

        static bool is_identity_transform(const aiMatrix4x4 &matrix);

        // Mesh processing
        static void process_mesh_vertices(aiMesh *mesh, std::vector<Vertex> &vertices,
                                          std::vector<unsigned int> &indices);

        static std::shared_ptr<Mesh> process_mesh(aiMesh *mesh, const aiScene *scene, const std::string &filepath);

        // Material processing
        static void preprocess_materials(const aiScene *scene, const std::string &filepath);

        static std::shared_ptr<Material> create_material(const aiMaterial *ai_material, const aiScene *scene,
                                                         const std::string &filepath);

        static void load_essential_material_properties(const aiMaterial *ai_material, Material &material);

        static void load_material_textures(const aiMaterial *ai_material, Material &material, const aiScene *scene,
                                           const std::string &filepath);

        static bool try_load_embedded_texture_unified(const std::string &path_str, const aiScene *scene, TextureType type,
                                                      Material &material);

        static bool try_load_external_texture_unified(const std::string &path_str, const std::string &filepath,
                                                      TextureType type,
                                                      Material &material);

        // Texture processing
        static bool try_load_embedded_texture(const std::string &path_str, const aiScene *scene, TextureType dcr_type,
                                              Material &material, const std::string &property_name);

        static bool try_load_external_texture(const std::string &path_str, const std::string &filepath,
                                              TextureType dcr_type, Material &material,
                                              const std::string &property_name);

        static std::shared_ptr<Texture> load_cached_texture(const std::string &path, TextureType type);

        // Utility methods
        static std::string create_mesh_key(aiMesh *mesh, const std::string &filepath);

        static std::string create_material_key(const aiMaterial *ai_material, const std::string &filepath,
                                               unsigned int material_index);

        static std::string capitalize_first(const std::string &str);

        static void debug_scene_info(const aiScene *scene, const std::string &filepath);


        static std::vector<std::string> get_texture_search_paths(const std::string &filepath);

    private:
        // Caching
        static std::unordered_map<std::string, std::shared_ptr<Mesh> > mesh_cache;
        static std::unordered_map<std::string, std::shared_ptr<Material> > material_cache;
        static std::unordered_map<std::string, std::shared_ptr<Texture> > texture_cache;
    };
}
