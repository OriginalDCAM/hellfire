//
// Created by denzel on 08/12/2025.
//

#include "AssetManager.h"

#include "hellfire/serializers/MaterialSerializer.h"
#include "hellfire/serializers/MeshSerializer.h"
#include "loaders/TextureLoader.h"

namespace hellfire {
AssetManager::AssetManager(AssetRegistry &registry) : registry_(registry) {}

std::shared_ptr<Mesh> AssetManager::get_mesh(const AssetID id) {
  // Check cache
  if (auto it = mesh_cache_.find(id); it != mesh_cache_.end()) {
    if (auto locked = it->second.lock()) {
      return locked;
    }
    mesh_cache_.erase(it);
  }

  // Load from disk
  auto meta = registry_.get_asset(id);
  if (!meta || meta->type != AssetType::MESH) {
    std::cerr << "Invalid mesh asset: " << id << std::endl;
    return nullptr;
  }

  auto mesh = MeshSerializer::load(registry_.get_absolute_path(id));
  if (!mesh) {
    std::cerr << "Failed to load mesh: " << std::endl;
    return nullptr;
  }

  if (mesh && !mesh->is_built()) {
    mesh->build();  
  }

  mesh_cache_[id] = mesh;
  return mesh;
}

std::shared_ptr<Material> AssetManager::get_material(const AssetID id) {
  if (auto it = material_cache_.find(id); it != material_cache_.end()) {
    if (auto locked = it->second.lock()) {
      return locked;
    }
    material_cache_.erase(it);
  }

  auto meta = registry_.get_asset(id);
  if (!meta || meta->type != AssetType::MATERIAL) {
    return nullptr;
  }

  auto data = MaterialSerializer::load_json(registry_.get_absolute_path(id));
  if (!data) {
    return nullptr;
  }

  // Convert MaterialData to Material, loading textures
  auto material = MaterialBuilder::create(data->name);
  material->set_diffuse_color(data->diffuse_color);
  material->set_ambient_color(data->ambient_color);
  material->set_specular_color(data->specular_color);
  material->set_emissive_color(data->emissive_color);
  material->set_opacity(data->opacity);
  material->set_shininess(data->shininess);
  material->set_metallic(data->metallic);
  material->set_roughness(data->roughness);

  for (const auto &[type, tex_id]: data->texture_assets) {
    if (auto tex = get_texture(tex_id, type)) {
      material->set_texture(tex, static_cast<int>(type));
    }
  }

  material_cache_[id] = material;
  return material;
}

Texture* AssetManager::get_texture(const AssetID id, const TextureType type = TextureType::DIFFUSE) {
  if (const auto it = texture_cache_.find(id); it != texture_cache_.end()) {
    if (it->second) {
      return it->second.get();
    }
    texture_cache_.erase(it);
  }

  auto meta = registry_.get_asset(id);
  if (!meta || meta->type != AssetType::TEXTURE) {
    return nullptr;
  }
        
  auto texture = TextureLoader::load(registry_.get_absolute_path(id).string(), type);

  if (!texture || !texture->is_valid()) {
    return nullptr;
  }

  texture_cache_[id] = std::move(texture);
  return texture_cache_[id].get();
}

Texture *AssetManager::get_texture(const std::filesystem::path &path,
                                   const TextureType type) {
  if (!std::filesystem::exists(path)) return nullptr;
      
  auto texture = TextureLoader::load(path, type);
  
  
  return texture.get();
}

void AssetManager::unload(const AssetID id) {
  mesh_cache_.erase(id);
  material_cache_.erase(id);
  texture_cache_.erase(id);
}

void AssetManager::clear_cache() {
  mesh_cache_.clear();
  material_cache_.clear();
  texture_cache_.clear();
}

void AssetManager::reload_modified() {
  for (const AssetID id : registry_.get_modified_assets()) {
    unload(id);
  }
  registry_.refresh_assets();
}

void AssetManager::save_modified() {
  for (auto &[id, mat] : material_cache_) {
    // if (MaterialSerializer::save_json(registry_.get_absolute_path(id), mat)) {
    //     
    // }
  }

}
} // hellfire