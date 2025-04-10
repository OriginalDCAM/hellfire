//
// Created by denzel on 09/04/2025.
//

#ifndef MODELLOADER_H
#define MODELLOADER_H
#include "assimp/scene.h"
#include "DCraft/Structs/Object3D.h"

namespace DCraft {
    class ImportedModel3D;
    class Shape3D;
    class Scene;
}

namespace DCraft::Addons {
    class ModelLoader {
    public:
        static ImportedModel3D* load(const std::string& filepath, Scene* target_scene);
    private:
        static ImportedModel3D* process_node(aiNode* node, const aiScene* scene, Material* default_material);
        static Mesh* process_mesh(aiMesh* mesh, const aiScene* scene, Material* default_material);
        
        struct MaterialMap {
            std::string type;
            std::string path;
        };
        static std::vector<MaterialMap> load_material_textures(aiMaterial* material, aiTextureType type, std::string typeName);
        static std::unordered_map<std::string, Mesh*> mesh_cache;
        static std::vector<MaterialMap> textures_loaded;

        
        
    };
}


#endif //MODELLOADER_H
