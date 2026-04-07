#include "DCraft/Structs/Camera.h"

#include <algorithm>
#include <glm/gtc/quaternion.hpp>

namespace DCraft {
    glm::mat4 Camera::get_view_matrix() {
        return glm::lookAt(get_position(), get_position() + front_, up_);
    }

    void Camera::process_keyboard(Camera_Movement direction, float delta_time) {
        float velocity = movement_speed_ * delta_time;
        glm::vec3 horizontal_front = glm::normalize(glm::vec3(front_.x, 0.0f, front_.z));
        glm::vec3 vertical_up = glm::normalize(glm::vec3(0.0f, up_.y, 0.0f));

        if (direction == FORWARD)
            set_position(get_position() + horizontal_front * velocity);
        if (direction == BACKWARD)
            set_position(get_position() - horizontal_front * velocity);
        if (direction == LEFT)
            set_position(get_position() - right_ * velocity);
        if (direction == RIGHT)
            set_position(get_position() + right_ * velocity);
        if (direction == DOWN)
            set_position(get_position() - vertical_up * velocity);
        if (direction == UP)
            set_position(get_position() + vertical_up * velocity);
    }

    void Camera::process_mouse_movement(float x_offset, float y_offset, bool constrain_pitch) {
        x_offset *= mouse_sensitivity_;
        y_offset *= mouse_sensitivity_;

        yaw_ += x_offset;
        pitch_ += y_offset;

        // Constrain pitch from -90 to 90 to prevent looking down or up too far
        if (constrain_pitch) {
            pitch_ = std::min(pitch_, 89.0f);
            pitch_ = std::max(pitch_, -89.0f);
        }

        // Update camera vectors
        update_camera_vectors();
    }

    void Camera::process_mouse_scroll(float y_offset) {
        zoom_ -= y_offset;
        zoom_ = std::max(zoom_, 1.0f);
        zoom_ = std::min(zoom_, 45.0f);
    }

    json Camera::to_json() {
        json j = Object3D::to_json();
        j["type"] = "camera";
        j["movement_speed"] = movement_speed_;
        j["mouse_sensitivity"] = mouse_sensitivity_;
        j["yaw"] = yaw_;
        j["pitch"] = pitch_;
        j["target"] = {target_.x, target_.y, target_.z };
        j["front"] = {front_.x, front_.y, front_.z};
        j["up"] = { up_.x, up_.y, up_.z};
        j["world_up"] = { world_up_.x, world_up_.y, world_up_.z};
        
        return j;
    }

    void Camera::update_camera_vectors() {
        // Calculate the new front vector
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front_ = glm::normalize(front);

        // Recalculate right and up vectors
        right_ = glm::normalize(glm::cross(front_, world_up_));
        up_ = glm::normalize(glm::cross(right_, front_));

        // Create a rotation matrix from the basis vectors
        glm::mat4 rotation(1.0f);
        rotation[0] = glm::vec4(right_, 0.0f);
        rotation[1] = glm::vec4(up_, 0.0f);
        rotation[2] = glm::vec4(front_, 0.0f);

        set_rotation_matrix(rotation);
    }
}
