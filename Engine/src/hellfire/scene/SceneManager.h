#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>


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

        // Setup callback for when a scene is activated
        using SceneActivatedCallback = std::function<void(Scene *)>;

        void set_scene_activated_callback(SceneActivatedCallback callback) {
            scene_activated_callback_ = callback;
        }

        // Scene creation and loading
        void create_default_scene();

        Scene *load_scene(const std::string &filename);

        bool save_scene(const std::string &filename, Scene *scene) const;

        // Scene management
        void update(float delta_time);

        void clear();

        // Object management

        Scene *create_scene(const std::string &name = "GameScene");

        EntityID find_entity_by_name(const std::string &name);

        // Accessor to objects
        void set_active_camera(EntityID camera) const;
        CameraComponent *get_active_camera() const;

        void set_active_scene(Scene *scene);

        void set_active_scene(const std::shared_ptr<Scene> &scene);

        std::vector<EntityID> get_camera_entities() const;

        Scene *get_active_scene() const {
            if (!active_scene_) return nullptr;

            return active_scene_;
        }

    private:
        std::vector<Scene*> scenes_;
        Scene *active_scene_;

        // Helper methods
        SceneActivatedCallback scene_activated_callback_;
    };
}
