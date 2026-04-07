#pragma once
#include <string>

#include "../ecs/Entity.h"
#include "nlohmann/json.hpp"


namespace hellfire
{
    class CameraComponent;
    class Skybox;

    class Scene 
    {
    public:
        Scene(const std::string& name = "Unnamed");
        virtual ~Scene();

        // Scene lifecycle
        virtual void initialize();
        virtual void update(float delta_time);

        // Entity management
        void add_entity(Entity* entity);
        void remove_entity(const Entity* entity);
        Entity* create_entity(const std::string& name = "GameObject");
        const std::vector<Entity*>& get_entities() const { return entities_; }

        // Find entities
        Entity* find_entity_by_name(const std::string& name);
        std::vector<Entity*> find_entities_with_component(const std::type_index& component_type);

        // Template version for type safety
        template<typename T>
        std::vector<Entity*> find_entities_with_component() {
            std::vector<Entity*> results;
            find_entities_with_component_recursive<T>(entities_, results);
            return results;
        }

        // Camera management
        void set_active_camera(Entity* camera_entity);
        Entity* get_active_camera_entity() const { return active_camera_entity_; }
        CameraComponent* get_active_camera() const {
            return active_camera_entity_ ? active_camera_entity_->get_component<CameraComponent>() : nullptr;
        }

        // Get all camera entities in the scene
        std::vector<Entity*> get_camera_entities() const;
        void destroy_camera(Entity* camera_entity);

        // Skybox management
        void set_skybox(Skybox* skybox) { skybox_ = skybox; }
        Skybox* get_skybox() const { return skybox_; }
        bool has_skybox() const { return skybox_ != nullptr; }

        // Scene properties
        const std::string& get_name() const { return name_; }
        void set_name(const std::string& name) { name_ = name; }
        
        bool is_active() const { return is_active_; }
        void set_active(bool active) { is_active_ = active; }

        void set_source_filename(const std::string& filename) { source_filename_ = filename; }
        const std::string& get_source_filename() const { return source_filename_; }
        bool was_loaded_from_file() const { return !source_filename_.empty(); }

        // World matrix updates for all entities
        void update_world_matrices() const;

        // Serialization
        nlohmann::json to_json();

    private:
        std::vector<Entity*> entities_;
        Entity* active_camera_entity_;
        std::string name_;
        bool is_active_;
        std::string source_filename_;
        Skybox* skybox_ = nullptr;

        // Helper methods
        void find_entity_by_name_recursive(const std::vector<Entity*>& entities, const std::string& name, Entity*& result);

        template<typename T>
        void find_entities_with_component_recursive(const std::vector<Entity*>& entities, std::vector<Entity*>& results) {
            for (Entity* entity : entities) {
                if (entity->has_component<T>()) {
                    results.push_back(entity);
                }

                // Recurse through children
                find_entities_with_component_recursive<T>(entity->get_children(), results);
            }
        }
    };

}

