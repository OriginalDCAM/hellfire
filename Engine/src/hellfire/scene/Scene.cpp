#include "Scene.h"

#include "../graphics/Skybox.h"
#include "hellfire/ecs/CameraComponent.h"

namespace hellfire {
    Scene::Scene(const std::string &name) : name_(name), is_active_(false), active_camera_entity_(nullptr),
                                            skybox_(nullptr) {
    }

    Scene::~Scene() {
        // Clean up all entities
        for (const Entity *entity: entities_) {
            delete entity;
        }

        entities_.clear();

        if (skybox_) {
            delete skybox_;
            skybox_ = nullptr;
        }
    }

    void Scene::initialize() {
        // Initialize all entities
        for (Entity *entity: entities_) {
            entity->setup();
        }
    }

    void Scene::update(float delta_time) {
        // Update all entities
        for (Entity *entity: entities_) {
            entity->update(delta_time);
        }

        // Update world matrices for the entire hierarchy
        update_world_matrices();
    }

    void Scene::add_entity(Entity *entity) {
        if (entity && std::find(entities_.begin(), entities_.end(), entity) == entities_.end()) {
            entities_.push_back(entity);
            entity->setup();
        }
    }

    void Scene::remove_entity(const Entity *entity) {
        auto it = std::find(entities_.begin(), entities_.end(), entity);
        if (it != entities_.end()) {
            // If this entity was the active camera, clear it
            if (entity == active_camera_entity_) {
                active_camera_entity_ = nullptr;
            }

            entities_.erase(it);
        }
    }

    Entity *Scene::create_entity(const std::string &name) {
        auto *entity = new Entity(name);
        add_entity(entity);
        return entity;
    }

    Entity *Scene::find_entity_by_name(const std::string &name) {
        Entity *result = nullptr;
        find_entity_by_name_recursive(entities_, name, result);
        return result;
    }

    void Scene::find_entity_by_name_recursive(const std::vector<Entity *> &entities,
                                              const std::string &name, Entity *&result) {
        if (result) return; // already found

        for (Entity *entity: entities) {
            if (entity->get_name() == name) {
                result = entity;
                return;
            }
            // Recurse through children
            find_entity_by_name_recursive(entity->get_children(), name, result);
            if (result) return;
        }
    }

    std::vector<Entity*> Scene::find_entities_with_component(const std::type_index& component_type) {
        std::vector<Entity*> results;
        // TODO:
        // This would need a more complex implementation to work with type_index
        // For now, use the template version instead
        return results;
    }

    void Scene::set_active_camera(Entity* camera_entity) {
        // Verify the entity has a camera component
        if (camera_entity && camera_entity->has_component<CameraComponent>()) {
            active_camera_entity_ = camera_entity;
        }
    }

    std::vector<Entity *> Scene::get_camera_entities() const {
        std::vector<Entity*> cameras;

        std::function<void(const std::vector<Entity*>&)> find_cameras =
            [&](const std::vector<Entity*>& entities) {
                for (Entity* entity : entities) {
                    if (entity->has_component<CameraComponent>()) {
                        cameras.push_back(entity);
                    }
                    // Recurse through children
                    find_cameras(entity->get_children());
                }
            };

        find_cameras(entities_);
        return cameras;
    }

    void Scene::update_world_matrices() const {
        // Update world matrices for all root entities
        // Their children will be updated recursively
        for (Entity* entity : entities_) {
            entity->update_world_matrices();
        }
    }

    nlohmann::json Scene::to_json() {
        nlohmann::json j;
        j["name"] = name_;
        j["type"] = "Scene";
        
        // Serialize entities
        nlohmann::json entities_array = nlohmann::json::array();
        for (Entity* entity : entities_) {
            // TODO: need to implement Entity::to_json()
            // entities_array.push_back(entity->to_json());
        }
        j["entities"] = entities_array;
        
        return j;
    }
}
