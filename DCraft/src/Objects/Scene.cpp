#include "DCraft/Structs/Scene.h"

#include <fstream>

#include "Dcraft/Graphics/Primitives/Cube.h"

namespace DCraft 
{
    Scene::Scene(const std::string& name) : Object3D(), is_active_(false), active_camera_(nullptr)
    {
        set_name(name);
    }

    Scene::~Scene()
    {
        cameras_.clear();
    }

    // Scene lifecycle
    void Scene::initialize()
    {

    }

    void Scene::update(float delta_time)
    {
        for (auto obj: registered_objects_) {
            obj->update(delta_time);
        }
        // Object3D::update(delta_time);
    }



    Object3D* Scene::create_object(const std::string& name = "GameObject")
    {
        Object3D* obj = new Object3D();
        obj->set_name(name);
        register_object(obj);
        add(obj);
        return obj;
    
    }

    void Scene::draw(const glm::mat4& view, const glm::mat4& projection, uint32_t shader_program, void* renderer_context = nullptr)
    {
        if (!is_active_ || !active_camera_) return;

        glm::mat4 camera_view = active_camera_->get_view_matrix();
        glm::mat4 camera_projection = active_camera_->get_projection_matrix();

        Object3D::draw(camera_view, camera_projection, shader_program, renderer_context);
    }

    void Scene::add(Object3D *obj) {
        Object3D::add(obj);

        // Call the setup method if it has been overridden
        obj->setup();
        
        register_object(obj);
    }

    void Scene::set_active_camera(Camera* camera)
    {
        if (camera) {
            // Add camera to the list if it's not there already
            if (std::find(cameras_.begin(), cameras_.end(), camera) == cameras_.end()) {
                cameras_.push_back(camera);
            }
            active_camera_ = camera;
        }
    }

    void Scene::destroy_camera(Camera* camera)
    {
    }

    json Scene::to_json() {
        json j = Object3D::to_json();
        return j;
    }

    void Scene::register_object(Object3D* obj)
    {
        if (obj && std::find(registered_objects_.begin(), registered_objects_.end(), obj) == registered_objects_.end())
        {
            registered_objects_.push_back(obj);
        }
    }

    void Scene::unregister_object(Object3D* obj)
    {
        auto it = std::find(registered_objects_.begin(), registered_objects_.end(), obj);
        if (it != registered_objects_.end())
        {
            registered_objects_.erase(it);
        }
    }
}
