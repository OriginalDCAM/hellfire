#include "SceneManager.h"
#include "Scene.h"
#include "CameraFactory.h"
#include "hellfire/ecs/CameraComponent.h"
#include <fstream>
#include <iostream>

#include "hellfire/ecs/ComponentRegistration.h"
#include "hellfire/serialization/SceneSerializer.h"

namespace hellfire {
    SceneManager::SceneManager() : active_scene_(nullptr) {
        // Serialization of scene components
        register_all_components();
    }

    SceneManager::~SceneManager() {
        clear();
    }

    std::vector<Scene *> SceneManager::get_scenes() {
        return scenes_;
    }

    void SceneManager::save_current_scene() const {
        save_scene(active_scene_->get_source_filename().string(), active_scene_);
    }

    Scene *SceneManager::create_scene(const std::string &name) {
        auto *scene = new Scene(name);
        scenes_.push_back(scene);
        return scene;
    }

    Scene *SceneManager::load_scene(const std::filesystem::path &filename) {
        // Check if already loaded
        for (Scene *scene: scenes_) {
            if (scene->get_source_filename() == filename) {
                std::cout << "Scene already loaded: " << filename << "\n";
                return scene;
            }
        }

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open scene file: " << filename << std::endl;
            return nullptr;
        }

        Scene* new_scene = create_scene();
        if (!Serializer<Scene>::deserialize(file, new_scene)) {
            std::cerr << "Failed to deserialize scene: " << filename << std::endl;
            destroy_scene(new_scene);
            return nullptr;
        }

        new_scene->set_source_filename(filename);
        return new_scene;
    }

    Scene* SceneManager::load_scene(AssetID asset_id, const std::filesystem::path& filename) {
        Scene* scene = load_scene(filename);
        if (scene) {
            scene_asset_ids_[scene] = asset_id;
        }
        return scene;
    }

    std::optional<AssetID> SceneManager::get_scene_asset_id(Scene* scene) const {
        auto it = scene_asset_ids_.find(scene);
        if (it != scene_asset_ids_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::optional<AssetID> SceneManager::get_active_scene_asset_id() const {
        return get_scene_asset_id(active_scene_);
    }

    bool SceneManager::save_scene(const std::string &filepath, Scene *scene) const {
        if (!scene) scene = get_active_scene();
        if (!scene) return false;

        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filepath << std::endl;
            return false;
        }

        if (!Serializer<Scene>::serialize(file, scene)) {
            std::cerr << "Failed to serialize scene: " << filepath << std::endl;
            return false;
        }

        scene->set_source_filename(filepath);
        return true;
    }

    void SceneManager::update(float delta_time) {
        if (active_scene_) {
            active_scene_->update(delta_time);
        }
    }

    void SceneManager::clear() {
        for (Scene* scene : scenes_) {
            delete scene;
        }
        scenes_.clear();
        scene_asset_ids_.clear(); 
        active_scene_ = nullptr;
    }

    void SceneManager::destroy_scene(Scene* scene) {
        if (!scene) return;
    
        // Remove from asset ID map
        scene_asset_ids_.erase(scene);
    
        // Remove from scenes vector
        auto it = std::find(scenes_.begin(), scenes_.end(), scene);
        if (it != scenes_.end()) {
            scenes_.erase(it);
        }
    
        // Clear active if this was it
        if (active_scene_ == scene) {
            active_scene_ = nullptr;
        }
    
        delete scene;
    }


    EntityID SceneManager::find_entity_by_name(const std::string &name) {
        if (active_scene_) {
            Entity *entity = active_scene_->find_entity_by_name(name);
            return entity ? entity->get_id() : 0;
        }
        return 0;
    }

    void SceneManager::set_active_camera(EntityID camera_id) const {
        if (active_scene_) {
            active_scene_->set_default_camera(camera_id);
        }
    }

    CameraComponent *SceneManager::get_active_camera() const {
        if (active_scene_) {
            return active_scene_->get_default_camera();
        }
        return nullptr;
    }

    std::vector<EntityID> SceneManager::get_camera_entities() const {
        if (active_scene_) {
            return active_scene_->get_camera_entities();
        }
        return {};
    }

    void SceneManager::set_active_scene(Scene *scene, const bool should_play) {
        if (scene == active_scene_) return;

        active_scene_ = scene;
        if (scene) {
            scene->set_playing(should_play);
            if (scene_activated_callback_) {
                scene_activated_callback_(scene);
            }
        }
    }
}
