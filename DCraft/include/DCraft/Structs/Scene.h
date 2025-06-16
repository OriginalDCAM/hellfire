#pragma once
#include "DCraft/Addons/PerspectiveCamera.h"
#include "DCraft/Structs/Camera.h"
#include "DCraft/Structs/Object3D.h"

namespace DCraft
{
    class Skybox;

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

        void draw(const glm::mat4& view, const glm::mat4& projection, uint32_t shader_program, void* renderer_context) override;

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
        std::vector<Camera*> get_cameras() {return cameras_; }

        void set_skybox(Skybox* skybox) { skybox_ = skybox; }
        Skybox* get_skybox() const { return skybox_; }
        bool has_skybox() const { return skybox_ != nullptr; }

        bool is_active() const { return is_active_; }
        void set_active(const bool active) { is_active_ = active; }

        void set_source_filename(const std::string& filename) { source_filename = filename; }
        const std::string& get_source_filename() const { return source_filename; }
        bool was_loaded_from_file() const { return !source_filename.empty(); }

        json to_json() override;

    private:
        std::vector<Object3D*> registered_objects_;
        std::vector<Camera*> cameras_;
        Camera* active_camera_;
        bool is_active_;
        std::string source_filename;
        Skybox* skybox_ = nullptr;

        void register_object(Object3D* obj);
        void unregister_object(Object3D* obj);
    };

}

