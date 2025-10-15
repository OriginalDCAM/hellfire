#include "Scene.h"

#include "../graphics/Skybox.h"
#include "hellfire/assets/AnimationSystem.h"
#include "hellfire/ecs/CameraComponent.h"

namespace hellfire {
    Scene::Scene(const std::string &name) : name_(name), is_active_(false), active_camera_entity_id_(0) {
    }

    Scene::~Scene() {
        entities_.clear();
    }

    EntityID Scene::create_entity(const std::string &name) {
        std::string unique_name = generate_unique_name(name);
        
        EntityID id = next_id_++;
        auto entity = std::make_unique<Entity>(id, unique_name);

        entity->add_component<TransformComponent>();
        
        entities_[id] = std::move(entity);
        root_entities_.push_back(id);

        // Initialize scripts
        entities_[id]->initialize_scripts();

        return id;
    }

    void Scene::destroy_entity(EntityID id) {
        const auto it = entities_.find(id);
        if (it == entities_.end()) return;

        // Cleanup scripts first
        it->second->cleanup_scripts();

        if (id == active_camera_entity_id_) {
            active_camera_entity_id_ = 0;
        }

        // Recursively destroy children
        const std::vector<EntityID> children = get_children(id);
        for (const EntityID child_id: children) {
            destroy_entity(child_id);
        }

        // Remove from parent's children list
        if (const auto parent_it = parent_map_.find(id); parent_it != parent_map_.end()) {
            const EntityID parent_id = parent_it->second;
            auto &siblings = children_map_[parent_id];
            siblings.erase(std::remove(siblings.begin(), siblings.end(), id), siblings.end());
            parent_map_.erase(parent_it);
        } else {
            // Remove from roots
            root_entities_.erase(std::remove(root_entities_.begin(), root_entities_.end(), id), root_entities_.end());
        }

        // Remove children mapping
        children_map_.erase(id);

        // Delete the entity
        entities_.erase(it);
    }

    Entity *Scene::get_entity(EntityID id) {
        const auto it = entities_.find(id);
        return it != entities_.end() ? it->second.get() : nullptr;
    }

    const Entity *Scene::get_entity(EntityID id) const {
        const auto it = entities_.find(id);
        return it != entities_.end() ? it->second.get() : nullptr;
    }

    void Scene::set_parent(EntityID child_id, EntityID parent_id) {
        if (entities_.find(child_id) == entities_.end()) return;
        if (parent_id != 0 && entities_.find(parent_id) == entities_.end()) return;

        // Remove from current parent
        if (auto it = parent_map_.find(child_id); it != parent_map_.end()) {
            EntityID old_parent = it->second;
            auto &siblings = children_map_[old_parent];
            siblings.erase(std::remove(siblings.begin(), siblings.end(), child_id), siblings.end());
        } else {
            // Was a root entity
            root_entities_.erase(std::remove(root_entities_.begin(), root_entities_.end(), child_id),
                                 root_entities_.end());
        }

        // Set new parent
        if (parent_id != 0) {
            parent_map_[child_id] = parent_id;
            children_map_[parent_id].push_back(child_id);
        } else {
            // Moving to root
            parent_map_.erase(child_id);
            root_entities_.push_back(child_id);
        }
    }

    void Scene::set_as_root(EntityID entity_id) {
        set_parent(entity_id, 0);
    }

    EntityID Scene::get_parent(EntityID entity_id) const {
        const auto it = parent_map_.find(entity_id);
        return it != parent_map_.end() ? it->second : 0;
    }

    std::vector<EntityID> Scene::get_children(EntityID parent_id) const {
        const auto it = children_map_.find(parent_id);
        return it != children_map_.end() ? it->second : std::vector<EntityID>{};
    }

    void Scene::initialize() {
        for (const EntityID root_id: root_entities_) {
            if (const Entity *entity = get_entity(root_id)) {
                entity->initialize_scripts();
            }
        }
    }

    void Scene::update(float delta_time) {
        for (const EntityID root_id: root_entities_) {
            update_hierarchy(root_id, delta_time);
        }
        update_world_matrices();
    }

    void Scene::update_world_matrices() {
        for (const EntityID root_id: root_entities_) {
            update_world_matrices_recursive(root_id, glm::mat4(1.0f));
        }
    }

    Entity *Scene::find_entity_by_name(const std::string &name) {
        for (const auto &[id, entity]: entities_) {
            if (entity->get_name() == name)
                return entity.get();
        }
        return nullptr;
    }

    void Scene::set_active_camera(const EntityID camera_id) {
        Entity* entity = get_entity(camera_id);
        if (entity && entity->has_component<CameraComponent>()) {
            active_camera_entity_id_ = camera_id;
        }
    }

    CameraComponent *Scene::get_active_camera() const {
        if (active_camera_entity_id_ == 0) return nullptr;
        const Entity* entity = const_cast<Scene*>(this)->get_entity(active_camera_entity_id_);
        return entity ? entity->get_component<CameraComponent>() : nullptr;
    }

    std::vector<EntityID> Scene::get_camera_entities() const {
        std::vector<EntityID> cameras;
        for (const auto& [id, entity] : entities_) {
            if (entity->has_component<CameraComponent>()) {
                cameras.push_back(id);
            }
        }
        return cameras;
    }

    void Scene::set_skybox(Skybox *skybox) {
        skybox_.reset(skybox);
    }

    std::string Scene::generate_unique_name(const std::string &base_name) {
        // Check if base name exists
        bool name_exists = false;
        for (const auto& [id, entity] : entities_) {
                if (entity->get_name() == base_name) {
                    name_exists = true;
                    break;
                }
        }

        // If base name is unique, use it
        if (!name_exists) {
            name_counters_[base_name] = 0;
            return base_name;
        }

        // Otherwise, find next available number
        int& counter = name_counters_[base_name];
        std::string unique_name;

        do {
            counter++;
            unique_name = base_name + " (" + std::to_string(counter) + ")";

            // Check if this numbered name exists
            name_exists = false;
            for (const auto &entity: entities_ | std::views::values) {
                if (entity->get_name() == unique_name) {
                    name_exists = true;
                    break;
                }
            }
        } while (name_exists);

        return unique_name;
    }

    void Scene::update_hierarchy(EntityID entity_id, float delta_time) {
        Entity* entity = get_entity(entity_id);
        if (!entity) return;

        entity->update_scripts(delta_time);

        for (EntityID child_id : get_children(entity_id)) {
            update_hierarchy(child_id, delta_time);
        }
    }

    void Scene::update_world_matrices_recursive(EntityID entity_id, const glm::mat4& parent_world) {
        Entity* entity = get_entity(entity_id);
        if (!entity) return;

        TransformComponent* transform = entity->get_component<TransformComponent>();
        if (!transform) {
            std::cerr << "CRITICAL: Entity '" << entity->get_name() 
                      << "' (ID: " << entity_id << ") missing TransformComponent!\n";
            assert(false);
            return; 
        }

        transform->update_local_matrix();
        transform->update_world_matrix(parent_world);

        // Recurse into children with this entity's world matrix
        const glm::mat4& this_world = transform->get_world_matrix();
        for (EntityID child_id : get_children(entity_id)) {
            update_world_matrices_recursive(child_id, this_world);
        }
    }

    void Scene::find_entities_recursive(EntityID entity_id, const std::function<bool(Entity *)> &predicate,
                                        std::vector<EntityID> &results) {
        Entity* entity = get_entity(entity_id);
        if (!entity) return;

        if (predicate(entity)) {
            results.push_back(entity_id);
        }

        for (EntityID child_id : get_children(entity_id)) {
            find_entities_recursive(child_id, predicate, results);
        }
    }
}
