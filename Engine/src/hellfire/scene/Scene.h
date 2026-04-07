#pragma once
#include <string>

#include "../ecs/Entity.h"
#include "glm/mat4x4.hpp"
#include "nlohmann/json.hpp"

static constexpr hellfire::EntityID INVALID_ENTITY = 0;

namespace hellfire {
    class CameraComponent;
    class Skybox;

    using EntityID = uint32_t;

    class Scene {
    public:
        Scene(const std::string &name = "Unnamed");

        virtual ~Scene();

        // Entity lifecycle
        EntityID create_entity(const std::string &name = "GameObject");

        void destroy_entity(EntityID id);

        Entity *get_entity(EntityID id);

        const Entity *get_entity(EntityID id) const;

        // Hierarchy management
        void set_parent(EntityID child_id, EntityID parent_id);

        void set_as_root(EntityID entity_id);

        EntityID get_parent(EntityID entity_id) const;

        std::vector<EntityID> get_children(EntityID parent_id) const;

        const std::vector<EntityID> &get_root_entities() const { return root_entities_; }

        // Scene lifecycle
        virtual void initialize();

        virtual void update(float delta_time);

        void update_world_matrices();

        // Finding entities
        Entity *find_entity_by_name(const std::string &name);

        template<typename T>
        std::vector<EntityID> find_entities_with_component();

        // Camera management
        void set_active_camera(EntityID camera_id);

        EntityID get_active_camera_entity() const { return active_camera_entity_id_; }

        CameraComponent *get_active_camera() const;

        std::vector<EntityID> get_camera_entities() const;

        // Skybox management
        void set_skybox(Skybox *skybox);

        Skybox *get_skybox() const { return skybox_.get(); }
        bool has_skybox() const { return skybox_ != nullptr; }

        // Scene properties
        const std::string &get_name() const { return name_; }
        void set_name(const std::string &name) { name_ = name; }
        bool is_active() const { return is_active_; }
        void set_active(bool active) { is_active_ = active; }
        void set_source_filename(const std::string &filename) { source_filename_ = filename; }
        const std::string &get_source_filename() const { return source_filename_; }
        bool was_loaded_from_file() const { return !source_filename_.empty(); }

    private:
        // All entities owned by scene
        std::unordered_map<EntityID, std::unique_ptr<Entity> > entities_;

        // Hierarchy management
        std::unordered_map<EntityID, EntityID> parent_map_;
        std::unordered_map<EntityID, std::vector<EntityID> > children_map_;
        std::vector<EntityID> root_entities_;

        // Scene state
        EntityID next_id_ = 1;
        EntityID active_camera_entity_id_ = 0;
        std::string name_;
        bool is_active_;
        std::string source_filename_;
        std::unique_ptr<Skybox> skybox_;

        // Helper methods
        void update_hierarchy(EntityID entity_id, float delta_time);

        void update_world_matrices_recursive(unsigned int entity_id, const glm::mat4& parent_world);

        void find_entities_recursive(EntityID entity_id, const std::function<bool(Entity *)> &predicate,
                                     std::vector<EntityID> &results);
    };

    template<typename T>
    std::vector<EntityID> Scene::find_entities_with_component() {
        std::vector<EntityID> results;
        auto predicate = [](Entity *entity) { return entity->has_component<T>(); };

        for (EntityID root_id: root_entities_) {
            find_entities_recursive(root_id, predicate, results);
        }

        return results;
    }
}
