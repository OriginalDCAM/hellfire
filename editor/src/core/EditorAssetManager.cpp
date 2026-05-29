//
// Created by denzel on 29/05/2026.
//

#include "EditorAssetManager.h"

#include "hellfire/assets/loaders/TextureLoader.h"
hellfire::Texture* EditorAssetManager::load_icon(
    const std::filesystem::path& path) {
  if (const auto it = icon_cache_.find(path); it != icon_cache_.end()) {
    if (it->second) {
      return it->second.get();
    }
    icon_cache_.erase(it);
  }
        
  auto texture = hellfire::TextureLoader::load(path, hellfire::TextureType::DIFFUSE);

  if (!texture || !texture->is_valid()) {
    return nullptr;
  }

  icon_cache_[path] = std::move(texture);
  return icon_cache_[path].get();
  
}