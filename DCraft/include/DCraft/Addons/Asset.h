//
// Created by denzel on 10/04/2025.
//

#pragma once
#include "ModelLoader.h"

namespace DCraft {
    class Asset {
    public:
        Asset() = default;
        static Entity* load(const std::filesystem::path& file_path) {
            // Simple for now only model loading later other types of asset loading like scenes.
            return Addons::ModelLoader::load_model(file_path).release();
        }
        
    };
}
