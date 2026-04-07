#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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
    static std::unique_ptr<Entity> load_model(const std::filesystem::path& filepath);
    private:
        enum class MaterialType {
            LAMBERT,
            PHONG,
            PBR
        };
    static std::unique_ptr<Entity> process_node(aiNode *node, const aiScene *scene, const std::string &filepath);

        static void process_mesh_vertices(aiMesh *mesh, std::vector<Vertex> &vertices, std::vector<unsigned int> &indices);

        static std::shared_ptr<Mesh> process_mesh(aiMesh *mesh, const aiScene *scene, const std::string &filepath);
    
        // Enhanced material creation methods
        static std::unique_ptr<Material> create_material_from_assimp(aiMaterial* ai_material);

        // Property loading methods
        static void load_material_properties(const aiMaterial* ai_material, Material& material);

        // Enhanced texture loading
        static void load_material_textures(const aiMaterial* ai_material, Material& material);

        // Utility
        static std::string capitalize_first(const std::string& str);
    private:
        static std::unordered_map<std::string, std::shared_ptr<Mesh>> mesh_cache;
    };
}
