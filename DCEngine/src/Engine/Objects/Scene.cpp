#include "Scene.h"

namespace DCraft 
{
    Scene::Scene(const std::string& name) : Object3D(), is_active_(false), active_camera_(nullptr)
    {
        set_name(name);
        register_object(this);
    }

    Scene::~Scene()
    {
        for (auto* camera : cameras_)
        {
            delete camera;
        }
        cameras_.clear();
    }

    // Scene lifecycle
    void Scene::initialize()
    {

    }

    void Scene::update(float delta_time)
    {
        if (!is_active_) return;
        Object3D::update(delta_time);
    }


    Object3D* Scene::create_object(const std::string& name = "GameObject")
    {
        Object3D* obj = new Object3D();
        obj->set_name(name);
        register_object(obj);
        add(obj);
        return obj;
    
    }

    void Scene::draw(const glm::mat4& view, const glm::mat4& projection, uint32_t shader_program)
    {
        if (!is_active_ || !active_camera_) return;

        glm::mat4 camera_view = active_camera_->get_view_matrix();
        glm::mat4 camera_projection = active_camera_->get_projection_matrix();

        Object3D::draw(camera_view, camera_projection, shader_program);
    }

    void Scene::set_active_camera(Camera* camera)
    {
        if (std::find(cameras_.begin(), cameras_.end(), camera) != cameras_.end())
        {
            active_camera_ = camera;
        }
    }

    void Scene::destroy_camera(Camera* camera)
    {
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
