#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>


// Forward declarations
namespace DCraft {
    class CameraComponent;
    class Scene;
    class Camera;
    class ObjectDeserializer;
    class ObjectSerializer;
    class Entity;
}

namespace DCraft {
    class SceneManager {
    public:
        SceneManager();

        ~SceneManager();

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

        Entity *find_entity_by_name(const std::string &name);

        // Accessor to objects
        std::vector<Camera *> get_cameras() const;

        void set_active_camera(Entity *camera) const;

        CameraComponent *get_active_camera() const;

        void set_active_scene(Scene *scene);

        void set_active_scene(const std::shared_ptr<Scene> &scene);

        std::vector<Entity *> get_camera_entities() const;

        Scene *get_active_scene() const {
            if (!active_scene_) return nullptr;

            return active_scene_;
        }

    private:
        std::vector<Scene*> scenes_;
        Scene *active_scene_;

        // std::unique_ptr<ObjectDeserializer> object_deserializer_;
        // std::unique_ptr<ObjectSerializer> object_serializer_;

        // Helper methods
        SceneActivatedCallback scene_activated_callback_;
    };
}
