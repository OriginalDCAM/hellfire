#include "SceneManager.h"
#include "Scene.h"
#include "CameraFactory.h"
#include "hellfire/ecs/CameraComponent.h"
#include <fstream>
#include <iostream>

namespace hellfire {
    SceneManager::SceneManager() : active_scene_(nullptr) {
    }

    SceneManager::~SceneManager() {
        clear();
    }

    std::vector<Scene *> SceneManager::get_scenes() {
        return scenes_;
    }

    Scene *SceneManager::create_scene(const std::string &name) {
        auto *scene = new Scene(name);
        scenes_.push_back(scene);
        return scene;
    }

    Scene *SceneManager::load_scene(const std::string &filename) {
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

        nlohmann::json scene_data;
        try {
            file >> scene_data;
        } catch (const std::exception &e) {
            std::cerr << "Failed to parse scene file: " << e.what() << std::endl;
            return nullptr;
        }

        // Validate scene data
        if (!scene_data.contains("name") || !scene_data.contains("version")) {
            std::cerr << "Invalid scene file format" << std::endl;
            return nullptr;
        }

        Scene *new_scene = create_scene(scene_data["name"]);
        new_scene->set_source_filename(filename);

        // TODO: Deserialize entities from scene_data["entities"]
        if (scene_data.contains("entities") && !scene_data["entities"].empty()) {
            // Deserialize entities here when implemented
            std::cout << "TODO: Deserialize " << scene_data["entities"].size() << " entities\n";
        }
        return new_scene;
    }

    bool SceneManager::save_scene(const std::string &filepath, Scene *scene) const {
        if (!scene) scene = get_active_scene();
        if (!scene) return false;

        try {
            json scene_data;
            scene_data["name"] = scene->get_name();
            scene_data["version"] = "1.0";

            json entities_array = json::array();

            // TODO: Iterate through scene entities
            // for (EntityID id : scene->get_all_entity_ids()) {
            //     Entity* entity = scene->get_entity(id);
            //     json entity_data = serialize_entity(entity);
            //     entities_array.push_back(entity_data);
            // }

            scene_data["entities"] = entities_array;

            std::ofstream file(filepath);
            if (!file.is_open()) {
                std::cerr << "Failed to open file for writing: " << filepath << std::endl;
                return false;
            }

            file << std::setw(4) << scene_data << std::endl;

            scene->set_source_filename(filepath);

            return true;
        } catch (const std::exception &e) {
            std::cerr << "Error saving scene: " << e.what() << std::endl;
            return false;
        }
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

    EntityID SceneManager::find_entity_by_name(const std::string &name) {
        if (active_scene_) {
            Entity *entity = active_scene_->find_entity_by_name(name);
            return entity ? entity->get_id() : 0;
        }
        return 0;
    }

    void SceneManager::set_active_camera(EntityID camera_id) const {
        if (active_scene_) {
            active_scene_->set_active_camera(camera_id);
        }
    }

    CameraComponent *SceneManager::get_active_camera() const {
        if (active_scene_) {
            return active_scene_->get_active_camera();
        }
        return nullptr;
    }

    std::vector<EntityID> SceneManager::get_camera_entities() const {
        if (active_scene_) {
            return active_scene_->get_camera_entities();
        }
        return {};
    }

    void SceneManager::set_active_scene(Scene *scene) {
        if (scene == active_scene_) return;

        active_scene_ = scene;
        if (scene) {
            scene->set_active(true);
            if (scene_activated_callback_) {
                scene_activated_callback_(scene);
            }
        }
    }
}
