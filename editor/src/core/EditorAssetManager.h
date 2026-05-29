//
// Created by denzel on 29/05/2026.
//

#pragma once
#include <filesystem>

#include "hellfire/graphics/texture/Texture.h"

class EditorAssetManager {
public:
  hellfire::Texture* load_icon(const std::filesystem::path& path);
private:
  std::unordered_map<std::filesystem::path, std::unique_ptr<hellfire::Texture>> icon_cache_;
};
