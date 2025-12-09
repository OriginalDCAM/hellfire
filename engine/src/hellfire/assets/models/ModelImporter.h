//
// Created by denzel on 08/12/2025.
//

#pragma once
#include "ImportResult.h"
#include "assimp/matrix4x4.h"
#include "assimp/scene.h"
#include "glm/detail/type_mat.hpp"
#include "hellfire/assets/AssetRegistry.h"
#include "hellfire/graphics/texture/Texture.h"

namespace hellfire {

    struct ImportSettings {
        bool generate_normals = true;
        bool generate_tangents = true;
        bool triangulate = true;
        bool flip_uvs = false;
        bool optimize_meshes = true;
        float scale_factor = 1.0f;
    };

    /**
     * @brief Converts external model formats (FBX, GLTF, OBJ) into internal assets
     * this runs once during asset import, NOT at runtime!
     */
    class ModelImporter {
    public:
        ModelImporter(AssetRegistry& registry, const std::filesystem::path& output_dir);

        ImportResult import(const std::filesystem::path& source_path, const ImportSettings& settings = {});

    private:
        AssetRegistry& registry_;
        std::filesystem::path output_dir_;

        // Current import state
        const aiScene* ai_scene_ = nullptr;
        std::filesystem::path source_path_;
        std::filesystem::path source_dir_;
        std::string base_name_;

        // Processing pipeline
        unsigned int build_import_flags(const ImportSettings& settings) const;
        
        // Node processing
        void process_node_hierarchy(const aiNode* node, ImportResult& result, 
                                   size_t parent_index = SIZE_MAX);
        ImportedNode convert_node(const aiNode* node) const;

        // Mesh processing
        AssetID process_mesh(const aiMesh* mesh, size_t mesh_index);
        
        // Material processing
        AssetID process_material(const aiMaterial* ai_mat, size_t material_index);
        AssetID process_texture(const std::string& texture_path, TextureType type);

        // Texture resolution
        std::optional<std::filesystem::path> resolve_texture_path(
            const std::string& texture_ref) const;
        bool is_embedded_texture(const std::string& path) const;
        std::filesystem::path extract_embedded_texture(size_t index);

        // Utility
        static glm::mat4 convert_matrix(const aiMatrix4x4& m);
        static bool is_identity(const aiMatrix4x4& m);
        std::string make_unique_name(const std::string& base, 
                                    const std::string& suffix, 
                                    size_t index) const;
    };
} // hellfire