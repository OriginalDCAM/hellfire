#pragma once

#include "DCraft/Structs/Object3D.h"
#include "DCraft/Structs/Camera.h"
#include "DCraft/Structs/Scene.h"

#include <vector>

#include "DCraft/Utility/ObjectDeserializer.h"
#include "DCraft/Utility/ObjectSerializer.h"

namespace DCraft {
    class Scene;

    class SceneManager {
    public:
        SceneManager();
        ~SceneManager();

        // Setup callback for when a scene is activated
        using SceneActivatedCallback = std::function<void(Scene*)>;
        void set_scene_activated_callback(SceneActivatedCallback callback) {
            scene_activated_callback_ = callback;
        }

        // Scene creation and loading
        void create_default_scene();

        Scene* load_scene(const std::string &filename);

        bool save_scene(const std::string &filename, Scene *scene);

        // Scene management
        void update(float delta_time);

        void clear();

        // Object management
        Object3D *create_object(const std::string &name = "GameObject");

        Scene *create_scene(const std::string &name = "GameScene");

        Object3D *find_object_by_name(const std::string &name);

        Object3D *find_object_by_name_recursive(Object3D *parent, const std::string &name);

        void destroy_object(Object3D *object);

        // Accessor to objects
        Object3D *get_root_node() const { return root_node_; }
        const std::vector<Object3D *> &get_objects() const { return objects_; }

        std::vector<Camera *> get_cameras() { return active_scene_->get_cameras(); }


        void set_active_camera(Camera *camera) {
            active_scene_->set_active_camera(camera);
        }

        Camera *get_active_camera() const {
            if (!active_scene_->get_active_camera()) return nullptr;

            return active_scene_->get_active_camera();
        }

        void set_active_scene(Scene *scene);

        void set_active_scene(std::shared_ptr<Scene> scene);

        Scene *get_active_scene() const {
            if (!active_scene_) return nullptr;

            return active_scene_;
        }

    private:
        Object3D *root_node_;
        std::vector<Object3D *> objects_;
        Scene *active_scene_;

        std::unique_ptr<ObjectDeserializer> object_deserializer_;
        std::unique_ptr<ObjectSerializer> object_serializer_;

        // Helper methods
        void register_object(Object3D *object);
        void unregister_object(Object3D *object);
        SceneActivatedCallback scene_activated_callback_;
        bool objects_contains_camera_component(Object3D *object, Camera *target_camera);
    };
}
