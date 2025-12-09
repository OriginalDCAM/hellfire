#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include "hellfire/assets/AssetRegistry.h"


// Forward declarations
namespace hellfire {
    class CameraComponent;
    class Scene;
    class Camera;
    class ObjectDeserializer;
    class ObjectSerializer;
    using EntityID = uint32_t;
}

namespace hellfire {
    class SceneManager {
    public:
        SceneManager();
        ~SceneManager();

        std::vector<Scene*> get_scenes();

        void save_current_scene();

        // Setup callback for when a scene is activated
        using SceneActivatedCallback = std::function<void(Scene *)>;

        void set_scene_activated_callback(SceneActivatedCallback callback) {
            scene_activated_callback_ = callback;
        }

        // Scene creation and loading
        void create_default_scene();

        Scene *load_scene(const std::filesystem::path &filename);

        Scene *load_scene(AssetID asset_id, const std::filesystem::path &filename);

        std::optional<AssetID> get_scene_asset_id(Scene* scene) const;
        std::optional<AssetID> get_active_scene_asset_id() const;

        bool save_scene(Scene *scene);

        void set_scene_asset_id(Scene* scene, AssetID id);

        bool save_scene_as(const std::string &filename, Scene *scene) ;

        // Scene management
        void update(float delta_time);

        void clear();

        void destroy_scene(Scene *scene);

        // Object management

        Scene *create_scene(const std::string &name = "GameScene");

        EntityID find_entity_by_name(const std::string &name);

        // Accessor to objects
        void set_active_camera(EntityID camera) const;
        CameraComponent *get_active_camera() const;

        void set_active_scene(Scene *scene, bool should_play = true);

        std::vector<EntityID> get_camera_entities() const;

        Scene *get_active_scene() const {
            if (!active_scene_) return nullptr;

            return active_scene_;
        }

        bool has_active_scene() const {
            return active_scene_;
        }

    private:
        std::vector<Scene*> scenes_;
        Scene *active_scene_;
        std::unordered_map<Scene*, AssetID> scene_asset_ids_;

        // Helper methods
        SceneActivatedCallback scene_activated_callback_;
    };
}
