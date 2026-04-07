#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "ImportedModel3D.h"
#include "assimp/scene.h"
#include "DCraft/Graphics/Materials/Material.h"
#include "DCraft/Graphics/Managers/ShaderManager.hpp"
#include "DCraft/Graphics/Mesh.h"
#include "DCraft/Graphics/Textures/Texture.h"


namespace DCraft {
    class Scene;
}

namespace DCraft::Addons {
    class ModelLoader {
    public:
        struct MaterialMap {
            std::string type;
            std::string path;
        };

    private:
        enum class MaterialType {
            LAMBERT,
            PHONG,
            PBR
        };

        static std::unordered_map<std::string, std::shared_ptr<Mesh>> mesh_cache;
        static std::vector<MaterialMap> textures_loaded;

    public:
        // Updated method signatures to include ShaderManager
        static ImportedModel3D* load(const std::string& filepath, Scene* target_scene, ShaderManager& shader_manager);
        
    private:
        static ImportedModel3D* process_node(aiNode* node, const aiScene* scene, 
                                           Material* default_material, ShaderManager& shader_manager);
        static std::shared_ptr<Mesh> process_mesh(aiMesh* mesh, const aiScene* scene, 
                                                 Material* default_material, ShaderManager& shader_manager);

        // Enhanced material creation methods
        static std::unique_ptr<Material> create_material_from_assimp(aiMaterial* ai_material, ShaderManager& shader_manager);
        static MaterialType determine_material_type(aiMaterial* ai_material);
        
        // Property loading methods
        static void load_lambert_properties(aiMaterial* ai_material, Material& material);
        static void load_phong_properties(aiMaterial* ai_material, Material& material);
        static void load_pbr_properties(aiMaterial* ai_material, Material& material);
        
        // Enhanced texture loading
        static void load_material_textures_enhanced(aiMaterial* ai_material, Material& material);
        static void load_texture_if_exists(aiMaterial* ai_material, aiTextureType type, 
                                          Material& material, const std::string& property_name);
        static Texture* get_or_load_texture(const std::string& path);
        
        // Legacy texture loading (keep for compatibility)
        static std::vector<MaterialMap> load_material_textures(aiMaterial* material, 
                                                              aiTextureType type, std::string typeName);
        
        // Utility
        static std::string capitalize_first(const std::string& str);
    };
}
