//
// Created by denzel on 08/12/2025.
//

#include "AssetManager.h"

#include "hellfire/serializers/MaterialSerializer.h"
#include "hellfire/serializers/MeshSerializer.h"

namespace hellfire {
    AssetManager::AssetManager(AssetRegistry &registry) : registry_(registry) {}

    std::shared_ptr<Mesh> AssetManager::get_mesh(AssetID id) {
        // Check cache
        if (auto it = mesh_cache_.find(id); it != mesh_cache_.end()) {
            return it->second;
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

        mesh_cache_[id] = mesh;
        return mesh;
    }

    std::shared_ptr<Material> AssetManager::get_material(AssetID id) {
        if (auto it = material_cache_.find(id); it != material_cache_.end()) {
            return it->second;
        }

        auto meta = registry_.get_asset(id);
        if (!meta || meta->type != AssetType::MATERIAL) {
            return nullptr;
        }

        auto data = MaterialSerializer::load(registry_.get_absolute_path(id));
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

        for (const auto &tex_id: data->texture_assets | std::views::values) {
            if (auto tex = get_texture(tex_id)) {
                material->set_texture(tex, 0);
            }
        }

        material_cache_[id] = material;
        return material;
    }

    std::shared_ptr<Texture> AssetManager::get_texture(AssetID id) {
        if (auto it = texture_cache_.find(id); it != texture_cache_.end()) {
            return it->second;
        }

        auto meta = registry_.get_asset(id);
        if (!meta || meta->type != AssetType::TEXTURE) {
            return nullptr;
        }

        auto texture = std::make_shared<Texture>(
            registry_.get_absolute_path(id).string()
        );

        if (!texture->is_valid()) {
            return nullptr;
        }

        texture_cache_[id] = texture;
        return texture;
    }

    void AssetManager::unload(AssetID id) {
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
} // hellfire