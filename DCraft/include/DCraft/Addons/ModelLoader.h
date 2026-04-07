#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "DCraft/Graphics/Vertex.h"
#include "DCraft/Graphics/Materials/Material.h"


namespace DCraft {
    class Entity;
    class Mesh;
    class Scene;
}

namespace DCraft::Addons {
    class ModelLoader {
    public:
        // Updated method signatures to include ShaderManager
        static std::unique_ptr<Entity> load_model(const std::filesystem::path &filepath, unsigned int import_flags = 0);

        static void clear_cache();

        static void print_cache_stats();

    private:
        enum class MaterialType {
            LAMBERT,
            PHONG,
            PBR
        };

        // Import flag presets
        struct ImportFlags {
            // For preview
            static constexpr unsigned int PREVIEW =
                    aiProcess_Triangulate |
                    aiProcess_JoinIdenticalVertices |
                    aiProcess_RemoveRedundantMaterials;

            // For runtime use (default)
            static constexpr unsigned int RUNTIME =
                    PREVIEW |
                    aiProcess_CalcTangentSpace |
                    aiProcess_ImproveCacheLocality |
                    aiProcess_OptimizeMeshes |
                    aiProcess_OptimizeGraph |
                    aiProcess_ValidateDataStructure;

            // Slower for best quality
            static constexpr unsigned int HIGH_QUALITY =
                    RUNTIME |
                    aiProcess_GenSmoothNormals |
                    aiProcess_CalcTangentSpace |
                    aiProcess_FlipUVs |
                    aiProcess_EmbedTextures;
        };

        // Node processing
        static std::unique_ptr<Entity> process_node(aiNode *node, const aiScene *scene, const std::string &filepath);

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

        // Texture processing
        static bool try_load_embedded_texture(const std::string &path_str, const aiScene *scene, TextureType dcr_type,
                                              Material &material, const std::string &property_name);

        static bool try_load_external_texture(const std::string &path_str, const std::string &filepath,
                                              TextureType dcr_type, Material &material,
                                              const std::string &property_name);

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
    };
}
