#pragma once
#include <string>

#include "../ecs/Entity.h"
#include "glm/mat4x4.hpp"
#include "glm/detail/type_vec3.hpp"
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

        bool is_descendant(EntityID potential_descendant, EntityID potential_ancestor);

        // Hierarchy management
        void set_parent(EntityID child_id, EntityID parent_id);

        void set_as_root(EntityID entity_id);

        EntityID get_parent(EntityID entity_id) const;

        bool has_parent(EntityID entity_id) const;

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
        void set_default_camera(EntityID camera_id);

        EntityID get_default_camera_entity_id() const { return default_camera_entity_id_; }

        CameraComponent *get_default_camera() const;

        std::vector<EntityID> get_camera_entities() const;

        size_t get_entity_count() const { return entities_.size(); }

        // Skybox management
        void set_skybox(Skybox *skybox);

        Skybox *get_skybox() const { return skybox_.get(); }
        bool has_skybox() const { return skybox_ != nullptr; }

        // Scene properties
        const std::string &get_name() const { return name_; }
        void set_name(const std::string &name) { name_ = name; }
        bool is_playing() const { return is_playing_; }
        void set_playing(bool active) { is_playing_ = active; }
        void set_source_filename(const std::filesystem::path &filename) { source_filename_ = filename; }
        const std::filesystem::path &get_source_filename() const { return source_filename_; }
        bool was_loaded_from_file() const { return !source_filename_.empty(); }
        std::string generate_unique_name(const std::string& base_name);

        void set_ambient_light(float intensity) {
            ambient_light_ = glm::vec3(intensity);
        }
        glm::vec3 get_ambient_light() const { return ambient_light_; }

        void save();

    private:
        // All entities owned by scene
        std::unordered_map<EntityID, std::unique_ptr<Entity> > entities_;

        // Hierarchy management
        std::unordered_map<EntityID, EntityID> parent_map_;
        std::unordered_map<EntityID, std::vector<EntityID> > children_map_;
        std::vector<EntityID> root_entities_;

        // Scene state
        EntityID next_id_ = 1;
        EntityID default_camera_entity_id_ = 0;
        std::string name_;
        bool is_playing_;
        std::filesystem::path source_filename_;
        std::unordered_map<std::string, int> name_counters_;

        
        std::unique_ptr<Skybox> skybox_;
        glm::vec3 ambient_light_ =  glm::vec3(0.4f);

        // Helper methods
        void update_hierarchy(EntityID entity_id, float delta_time);

        void update_world_matrices_recursive(unsigned int entity_id, const glm::mat4 &parent_world);

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
