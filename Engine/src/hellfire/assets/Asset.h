//
// Created by denzel on 10/04/2025.
//

#pragma once
#include "ModelLoader.h"

namespace hellfire {
    class Scene;
    using EntityID = uint32_t;
    
    class Asset {
    public:
        Asset() = default;
        
        static EntityID load(Scene* scene, const std::filesystem::path& file_path, const unsigned int import_flags = 0) {
            return Addons::ModelLoader::load_model(scene, file_path, import_flags);
        }
        
    };
}
