#include "SceneManager.h"
#include "Scene.h"
#include "CameraFactory.h"
#include "hellfire/ecs/CameraComponent.h"
#include <fstream>
#include <iostream>

namespace hellfire {
    SceneManager::SceneManager() : active_scene_(nullptr) {}

    SceneManager::~SceneManager() {
        clear();
    }

    Scene* SceneManager::create_scene(const std::string& name) {
        auto* scene = new Scene(name);
        scenes_.push_back(scene);
        return scene;
    }

    Scene* SceneManager::load_scene(const std::string& filename) {
        // Check if already loaded
        for (Scene* scene : scenes_) {
            if (scene->get_source_filename() == filename) {
                std::cout << "Scene already loaded: " << filename << "\n";
                return scene;
            }
        }

        // Open and parse JSON file
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open scene file: " << filename << std::endl;
            return nullptr;
        }

        nlohmann::json scene_data;
        try {
            file >> scene_data;
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse scene file: " << e.what() << std::endl;
            return nullptr;
        }

        // Create scene
        std::string scene_name = scene_data.contains("name") ? scene_data["name"] : "Untitled Scene";
        Scene* new_scene = create_scene(scene_name);
        new_scene->set_source_filename(filename);

        // TODO: Deserialize entities from scene_data
        // For now, create a default camera if none exists
        EntityID camera_id = PerspectiveCamera::create(new_scene, "Main Camera", 
                                                       45.0f, 16.0f/9.0f, 0.1f, 100.0f,
                                                       glm::vec3(0, 0, 10));
        new_scene->set_active_camera(camera_id);

        return new_scene;
    }

    bool SceneManager::save_scene(const std::string& filepath, Scene* scene) const {
        if (!scene) scene = get_active_scene();
        if (!scene) return false;

        try {
            json scene_data;
            scene_data["name"] = scene->get_name();
            scene_data["version"] = "1.0";

            // TODO: Serialize entities
            json entities_array = json::array();
            // Iterate through scene entities and serialize them
            
            scene_data["entities"] = entities_array;

            std::ofstream file(filepath);
            if (!file.is_open()) {
                std::cerr << "Failed to open file for writing: " << filepath << std::endl;
                return false;
            }

            file << std::setw(4) << scene_data << std::endl;
            return true;
        } catch (const std::exception& e) {
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
        for (Scene* scene : scenes_) {
            delete scene;
        }
        scenes_.clear();
        active_scene_ = nullptr;
    }

    EntityID SceneManager::find_entity_by_name(const std::string& name) {
        if (active_scene_) {
            Entity* entity = active_scene_->find_entity_by_name(name);
            return entity ? entity->get_id() : 0;
        }
        return 0;
    }

    void SceneManager::set_active_camera(EntityID camera_id) const {
        if (active_scene_) {
            active_scene_->set_active_camera(camera_id);
        }
    }

    CameraComponent* SceneManager::get_active_camera() const {
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

    void SceneManager::set_active_scene(Scene* scene) {
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