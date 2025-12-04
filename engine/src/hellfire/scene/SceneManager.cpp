#include "SceneManager.h"
#include "Scene.h"
#include "CameraFactory.h"
#include "hellfire/ecs/CameraComponent.h"
#include <fstream>
#include <iostream>

#include "hellfire/serialization/SceneSerializer.h"

namespace hellfire {
    SceneManager::SceneManager() : active_scene_(nullptr) {
    }

    SceneManager::~SceneManager() {
        clear();
    }

    std::vector<Scene *> SceneManager::get_scenes() {
        return scenes_;
    }

    void SceneManager::save_current_scene() const {
        active_scene_->save();
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
        for (Scene *scene: scenes_) {
            delete scene;
        }
        scenes_.clear();
        active_scene_ = nullptr;
    }

    void SceneManager::destroy_scene(Scene* scene) {
        // TO
        
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
