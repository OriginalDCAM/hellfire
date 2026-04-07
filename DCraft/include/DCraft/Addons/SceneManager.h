#pragma once

#include "DCraft/Structs/Object3D.h"
#include "DCraft/Structs/Camera.h"

namespace DCraft
{
    class Scene;

    class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		// Scene creation and loading
		void create_default_scene();
		bool load_scene(const std::string& filename);
		bool save_scene(const std::string& filename);

		// Scene management
		void update(float delta_time);
		void clear();

		// Object management
		Object3D* create_object(const std::string& name = "GameObject");
		Scene* create_scene(const std::string& name = "GameScene");
		Object3D* find_object_by_name(const std::string& name);
		Object3D* find_object_by_name_recursive(Object3D* parent, const std::string& name);
		void destroy_object(Object3D* object);

		// Accessor to objects
		Object3D* get_root_node() const { return root_node_; }
		const std::vector<Object3D*>& get_objects() const { return objects_; }

		void set_active_camera(Camera* camera)
		{
			active_camera_ = camera;
		}

		Camera* get_active_camera() const
        {
			if (!active_camera_) return nullptr;

			return active_camera_;
		}

		void set_active_scene(Scene* scene);
	private:
		Object3D* root_node_;
		std::vector<Object3D*> objects_;
		Scene* active_scene_;
		Camera* active_camera_;

		// Helper methods
		void register_object(Object3D* object);
		void unregister_object(Object3D* object);
		bool objects_contains_camera_component(Object3D* object, Camera* target_camera);
	};

}
