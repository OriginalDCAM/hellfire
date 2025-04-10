#pragma once
#include <glm/detail/type_mat.hpp>

#include "DCraft/Structs/Object3D.h"

namespace DCraft
{

    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        DOWN,
        UP
    };

    // Default camera values
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 5.0f;
    const float SENSITIVITY = 0.1f;
    const float ZOOM = 45.0f;

    // Base Camera class that inherits from Object3D
    class Camera : public Object3D
    {
    protected:
        // camera Attributes
        glm::vec3 front_;
        glm::vec3 up_;
        glm::vec3 right_;
        glm::vec3 world_up_;
        glm::vec3 target_ = glm::vec3(0.0f);

        // euler Angles
        float yaw_;
        float pitch_;

        // camera options
        float movement_speed_;
        float mouse_sensitivity_;
        float zoom_;

    public:
        // Constructor
        Camera(const std::string& name = "UnnamedCamera", glm::vec3 position = glm::vec3(0.0f, 0.0f, -5.0f),
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
            float yaw = YAW,
            float pitch = PITCH) :
            front_(glm::vec3(0.0f, 0.0f, -1.0f)),
            world_up_(up),
            yaw_(yaw),
            pitch_(pitch),
            movement_speed_(SPEED),
            mouse_sensitivity_(SENSITIVITY),
            zoom_(ZOOM)
        {
            set_name(name);
            set_position(position);
            update_camera_vectors();
        }

        void set_movement_speed(float value) { movement_speed_ = value; }
        float get_movement_speed() { return movement_speed_; }

        virtual glm::mat4 get_view_matrix();
        virtual glm::mat4 get_projection_matrix() = 0;

        void process_keyboard(Camera_Movement direction, float delta_time);
        void process_mouse_movement(float x_offset, float y_offset, bool constrain_pitch = true);
        virtual void process_mouse_scroll(float y_offset);

        float get_zoom() const { return zoom_; }

        float get_pitch() const { return pitch_; }
        float get_yaw() const { return yaw_; }

        glm::vec3 get_front_vector() const { return front_; }
        glm::vec3 get_up_vector() const { return up_; }
        glm::vec3 get_right_vector() const { return right_; }
        glm::vec3 get_world_up_vector() const { return world_up_; }
        glm::vec3 get_camera_target() const { return target_; }

        // Override from Object3D
        void update(float dt) override {}

        void draw_self(const glm::mat4& view, const glm::mat4& projection, uint32_t shader_program, void* renderer_context) override {}

        virtual void set_aspect_ratio(float aspect_ratio) = 0;
        virtual float get_aspect_ratio() const = 0;

    protected:
        // Update camera vectors based on Euler angles
        void update_camera_vectors();
    };
}
