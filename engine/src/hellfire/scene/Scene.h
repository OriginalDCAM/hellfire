#pragma once
#include <string>

#include "SceneEnvironment.h"
#include "../ecs/Entity.h"
#include "glm/mat4x4.hpp"
#include "glm/detail/type_vec3.hpp"
#include "nlohmann/json.hpp"

static constexpr hellfire::EntityID INVALID_ENTITY = 0;

namespace hellfire {
    class CameraComponent;

    using EntityID = uint32_t;

    /**
     * @brief Manages a collection of entities and their hierarchical relationships
     *
     * The Scene class represents a complete game scene containing entities,
     * their parent-child relationships, cameras, and environmental settings.
     * It handles entity lifecycle, hierarchy management, and scene updates.
     */
    class Scene {
    public:
        /**
         * @brief Constructs a new Scene with an optional name
         * @param name The name of the scene (default: "Unnamed")
         */
        Scene(std::string name = "Unnamed");

        /**
         * @brief Destructor
         */
        virtual ~Scene();

        /**
         * @brief Creates a new entity in the scene
         * @param name The name for the new entity (default: "GameObject")
         * @return The unique ID of the newly created entity
         */
        EntityID create_entity(const std::string &name = "GameObject");

        /**
         * @brief Destroys an entity and removes it from the scene
         * @param id The ID of the entity to destroy
         */
        void destroy_entity(EntityID id);

        /**
         * @brief Retrieves an entity by its ID
         * @param id The ID of the entity to retrieve
         * @return Pointer to the entity, or nullptr if not found
         */
        Entity *get_entity(EntityID id);

        /**
         * @brief Retrieves an entity by its ID (const version)
         * @param id The ID of the entity to retrieve
         * @return Const pointer to the entity, or nullptr if not found
         */
        const Entity *get_entity(EntityID id) const;

        /**
         * @brief Checks if one entity is a descendant of another
         * @param potential_descendant The ID of the potential descendant entity
         * @param potential_ancestor The ID of the potential ancestor entity
         * @return True of potential_descendant is a descendant of potential_ancestor
         */
        bool is_descendant(EntityID potential_descendant, EntityID potential_ancestor);

        // Hierarchy management

        /**
         * @brief Sets the parent of an entity
         * @param child_id The ID of the child entity
         * @param parent_id The ID of the parent entity
         */
        void set_parent(EntityID child_id, EntityID parent_id);

        /**
         * @brief Makes an entity a root entity (removes parent)
         * @param entity_id The ID of the entity to make root
         */
        void set_as_root(EntityID entity_id);

        /**
         * @brief Gets the parent ID of an entity
         * @param entity_id The ID of the entity
         * @return The parent entity ID, or INVALID_ENTITY if no parent
         */
        EntityID get_parent(EntityID entity_id) const;

        /**
         * @brief Checks if an entity has a parent
         * @param entity_id The ID of the entity to check
         * @return True if the entity has a parent
         */
        bool has_parent(EntityID entity_id) const;

        /**
         * @brief Gets all children of a parent entity
         * @param parent_id The ID of the parent entity
         * @return Vector of child entity IDs
         */
        std::vector<EntityID> get_children(EntityID parent_id) const;

        /**
         * @brief Gets all root entities in the scene
         * @return Reference to the vector of root entity IDs
         */
        const std::vector<EntityID> &get_root_entities() const { return root_entities_; }

        // Scene lifecycle

        /**
         * @brief  Initializes the scene
         * Called once when the scene is first loaded or created
         */
        virtual void initialize();

        /**
         * @brief Updates all entities in the scene
         * @param delta_time Time elapsed since last update in seconds
         */
        virtual void update(float delta_time);

        /**
         * @brief Updates world transformation matrices for all entities
         * Propagates transformations through the entity hierarchy
         */
        void update_world_matrices();

        // Finding entities

        /**
         * @brief Finds an entity by its name
         * @param name The name of the entity to find
         * @return Pointer to the entity, or nullptr if not found
         */
        Entity *find_entity_by_name(const std::string &name);

        /**
         * @brief Finds all entities that have a specific component type
         * @tparam T The component type to search for
         * @return Vector of entity IDs that have specified component
         */
        template<typename T>
        std::vector<EntityID> find_entities_with_component();

        // Camera management

        /**
         * @brief Sets the default camera for the scene
         * @param camera_id 
         */
        void set_default_camera(EntityID camera_id);

        EntityID get_default_camera_entity_id() const { return default_camera_entity_id_; }

        CameraComponent *get_default_camera() const;

        std::vector<EntityID> get_camera_entities() const;

        size_t get_entity_count() const { return entities_.size(); }



        // Scene properties
        const std::string &get_name() const { return name_; }
        void set_name(const std::string &name) { name_ = name; }
        bool is_playing() const { return is_playing_; }
        void set_playing(bool active) { is_playing_ = active; }
        void set_source_filename(const std::filesystem::path &filename) { source_filename_ = filename; }
        const std::filesystem::path &get_source_filename() const { return source_filename_; }
        bool was_loaded_from_file() const { return !source_filename_.empty(); }
        std::string generate_unique_name(const std::string& base_name);

        void save();

        SceneEnvironment* environment() const { return environment_.get(); }

        std::unordered_map<EntityID, std::unique_ptr<Entity>>& get_all_entities() { return entities_; }

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

        std::unique_ptr<SceneEnvironment> environment_;

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
