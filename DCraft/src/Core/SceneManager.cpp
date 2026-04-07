#include "DCraft/Addons/SceneManager.h"

#include <fstream>

#include "DCraft/Addons/Cameras.h"
#include "DCraft/Utility/ObjectDeserializer.h"
#include "DCraft/Utility/ObjectSerializer.h"

#include "DCraft/Components/CameraComponent.h"
#include "DCraft/Graphics/Geometry/Cube.h"
#include "DCraft/Structs/Scene.h"

namespace DCraft {
    SceneManager::SceneManager() : active_scene_(nullptr) {
        // object_deserializer_ = std::make_unique<ObjectDeserializer>(this, nullptr);
        // object_serializer_ = std::make_unique<ObjectSerializer>(this);
    }

    SceneManager::~SceneManager() {
        clear();
    }

    void SceneManager::create_default_scene() {
        clear();

        Scene *default_scene = create_scene("DefaultScene");
        set_active_scene(default_scene);

        // Create camera entity
        auto *camera_entity = default_scene->create_entity("MainCamera");
        auto *camera_component = camera_entity->add_component<CameraComponent>();
        camera_component->set_perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
        camera_entity->transform()->set_position(0, 0, 10);

        default_scene->set_active_camera(camera_entity);

        auto *cube_entity = Cube::create("Cube");
        default_scene->add_entity(cube_entity);
    }

    Scene *SceneManager::load_scene(const std::string &filename) {
        // Check if already loaded
        for (Scene *scene: scenes_) {
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
        } catch (const std::exception &e) {
            std::cerr << "Failed to parse scene file: " << e.what() << std::endl;
            return nullptr;
        }

        // Create scene
        std::string scene_name = scene_data.contains("name") ? scene_data["name"] : "Untitled Scene";
        Scene *new_scene = create_scene(scene_name);
        new_scene->set_source_filename(filename);

        // Load entities
        if (scene_data.contains("entities") && scene_data["entities"].is_array()) {
            for (const auto &entity_data: scene_data["entities"]) {
                // TODO: deserialize entity_data into entity object
            }
        }

        // Find and set active camera
        Entity *main_camera = new_scene->find_entity_by_name("Main Camera");
        if (main_camera && main_camera->has_component<CameraComponent>()) {
            new_scene->set_active_camera(main_camera);
        } else {
            auto *default_camera = PerspectiveCamera::create("DefaultCamera", 45.0f, 16.0f / 9.0f, 0.1f, 100.0f,
                                                             glm::vec3(0, 0, 10));
            new_scene->add_entity(default_camera);
            new_scene->set_active_camera(default_camera);
        }

        return new_scene;
    }

    bool SceneManager::save_scene(const std::string &filepath, Scene *scene = nullptr) const {
        if (!scene) scene = get_active_scene();

        try {
            json scene_data;

            // Basic scene info
            scene_data["name"] = scene->get_name();
            scene_data["version"] = "1.0";

            // Scene objects
            json entities_array = json::array();
            auto &scene_entities = scene->get_entities();

            for (auto *entity: scene_entities) {
                if (entity) {
                    // entities_array.push_back(object_serializer_->serialize_object(entity));
                }
            }

            scene_data["entities"] = entities_array;

            // Write to file
            std::ofstream file(filepath);
            if (!file.is_open()) {
                std::cerr << "Failed to open file for writing: " << filepath << std::endl;
                return false;
            }

            // Write pretty-printed JSON
            file << std::setw(4) << scene_data << std::endl;

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

    Scene *SceneManager::create_scene(const std::string &name) {
        auto scene = new Scene(name);
        scenes_.push_back(scene);
        return scene;
    }

    Entity *SceneManager::find_entity_by_name(const std::string &name) {
        if (active_scene_) {
            return active_scene_->find_entity_by_name(name);
        }
        return nullptr;
    }

    void SceneManager::set_active_camera(Entity *camera) const {
        if (active_scene_) {
            active_scene_->set_active_camera(camera);
        }
    }

    CameraComponent *SceneManager::get_active_camera() const {
        if (active_scene_) {
            return active_scene_->get_active_camera();
        }
        return nullptr;
    }

    void SceneManager::set_active_scene(Scene *scene) {
        // Early exit to prevent having to perform an unnecessary search
        if (scene == active_scene_) return;

        active_scene_ = scene;
        if (scene) {
            scene->set_active(true);
            if (scene_activated_callback_) {
                scene_activated_callback_(scene);
            }
        }
    }

    void SceneManager::set_active_scene(const std::shared_ptr<Scene> &scene) {
        if (scene.get() != active_scene_) {
            active_scene_ = scene.get();
            if (scene) {
                scene->set_active(true);
                if (scene_activated_callback_) {
                    scene_activated_callback_(scene.get());
                }
            }
        }
    }

    std::vector<Entity*> SceneManager::get_camera_entities() const {
        if (active_scene_) {
            return active_scene_->get_camera_entities();
        }
        return std::vector<Entity*>();
    }
}
