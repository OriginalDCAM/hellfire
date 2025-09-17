//
// Created by denzel on 03/04/2025.
//

#include "DCraft/Graphics/Managers/TextureManager.h"

// Singleton class instance
DCraft::TextureManager &DCraft::TextureManager::get_instance() {
    if (!instance_) {
        instance_ = new TextureManager();
    }
    return *instance_;
}

DCraft::Texture *DCraft::TextureManager::get_texture(const std::string &path) {
    auto it = textures_.find(path);
    if (it != textures_.end()) {
        return it->second;
    }
    return nullptr;
}

void DCraft::TextureManager::clear() {
    for (auto &pair: textures_) {
        delete pair.second;
    }
    textures_.clear();
}

DCraft::TextureManager *DCraft::TextureManager::instance_ = nullptr;
