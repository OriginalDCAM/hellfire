//
// Created by denzel on 03/04/2025.
//

#include "DCraft/Graphics/Managers/TextureManager.h"

// Singleton class instance
hellfire::TextureManager &hellfire::TextureManager::get_instance() {
    if (!instance_) {
        instance_ = new TextureManager();
    }
    return *instance_;
}

hellfire::Texture *hellfire::TextureManager::get_texture(const std::string &path) {
    auto it = textures_.find(path);
    if (it != textures_.end()) {
        return it->second;
    }
    return nullptr;
}

void hellfire::TextureManager::clear() {
    for (auto &pair: textures_) {
        delete pair.second;
    }
    textures_.clear();
}

hellfire::TextureManager *hellfire::TextureManager::instance_ = nullptr;
