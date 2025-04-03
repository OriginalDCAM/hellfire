#pragma once
#include "DCraft/Addons/PerspectiveCamera.h"
#include "DCraft/Structs/Camera.h"
#include "DCraft/Structs/Object3D.h"

namespace DCraft
{
    class Scene : public Object3D
    {
    public:
        Scene(const std::string& name = "Unnamed");
        virtual ~Scene();

        // Scene lifecycle
        virtual void initialize();
        virtual void update(float delta_time) override;

        // Object management
        Object3D* create_object(const std::string& name);

        virtual void draw(const glm::mat4& view, const glm::mat4& projection, uint32_t shader_program) override;

        template<typename CameraType, typename... Args>
        CameraType* create_camera(const std::string& name, Args&& ... args)
        {
            CameraType* camera = new CameraType(name, std::forward<Args>(args)...);

            cameras_.push_back(camera);

            if (!active_camera_)
            {
                active_camera_ = camera;
            }

            return camera;
        }

        void add(Object3D* obj) override;
        
        void set_active_camera(Camera* camera);
        Camera* get_active_camera() const { return active_camera_; }
        void destroy_camera(Camera* camera);

        bool is_active() const { return is_active_; }
        void set_active(const bool active) { is_active_ = active; }

    private:
        std::vector<Object3D*> registered_objects_;
        std::vector<Camera*> cameras_;
        Camera* active_camera_;
        bool is_active_;

        void register_object(Object3D* obj);
        void unregister_object(Object3D* obj);
    };

}

