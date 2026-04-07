//
// Created by denzel on 22/08/2025.
//

#include "DCraft/Components/CameraComponent.h"

#include "DCraft/Structs/Entity.h"

namespace hellfire {
    CameraComponent::CameraComponent(CameraType type): camera_type_(type) {
        update_camera_vectors();
    }

    void CameraComponent::set_camera_type(CameraType type) {
        camera_type_ = type;
        projection_dirty_ = true;
    }

    void CameraComponent::set_perspective(float fov, float aspect, float near_plane, float far_plane) {
        camera_type_ = CameraType::PERSPECTIVE;
        fov_ = fov;
        aspect_ratio_ = aspect;
        near_plane_ = near_plane;
        far_plane_ = far_plane;
        projection_dirty_ = true;
    }

    void CameraComponent::set_orthographic(float left, float right, float bottom, float top, float near_plane,
        float far_plane) {
        camera_type_ = CameraType::ORTHOGRAPHIC;
        ortho_left_ = left;
        ortho_right_ = right;
        ortho_bottom_ = bottom;
        ortho_top_ = top;
        near_plane_ = near_plane;
        far_plane_ = far_plane;
        projection_dirty_ = true;
    }

    glm::mat4 CameraComponent::get_view_matrix() const {
        if (view_dirty_) {
            update_view_matrix();
        }
        return view_matrix_;
    }

    glm::mat4 CameraComponent::get_projection_matrix() const {
        if (projection_dirty_) {
            update_projection_matrix();
        }
        return projection_matrix_;
    }

    void CameraComponent::set_fov(float fov) {
        fov_ = fov;
        projection_dirty_ = true;
    }

    void CameraComponent::set_aspect_ratio(float aspect) {
        aspect_ratio_ = aspect;
        projection_dirty_ = true;
    }

    void CameraComponent::set_clip_planes(float near_plane, float far_plane) {
        near_plane_ = near_plane;
        far_plane_ = far_plane;
        projection_dirty_ = true;
    }

    void CameraComponent::set_yaw(float yaw) { 
        camera_data_.yaw = yaw; 
        update_camera_vectors();
        view_dirty_ = true;
    }

    void CameraComponent::set_pitch(float pitch) { 
        camera_data_.pitch = glm::clamp(pitch, -89.0f, 89.0f);
        update_camera_vectors();
        view_dirty_ = true;
    }

    void CameraComponent::set_mouse_sensitivity(float sensitivity) { 
        camera_data_.sensitivity = sensitivity; 
    }

    void CameraComponent::look_at(const glm::vec3 &target) {
        if (!get_owner()) return;

        auto *transform = get_owner()->get_component<TransformComponent>();
        if (!transform) return;

        target_ = target;
        glm::vec3 position = transform->get_position();
        glm::vec3 direction = glm::normalize(target - position);

        camera_data_.pitch = glm::degrees(asin(direction.y));
        camera_data_.yaw = glm::degrees(atan2(direction.z, direction.x));

        update_camera_vectors();
        view_dirty_ = true;
    }

    void CameraComponent::set_target(const glm::vec3 &target) { 
        target_ = target;
        look_at(target);
    }

    CameraComponent * CameraComponent::create_perspective(float fov, float aspect, float near_plane, float far_plane) {
        auto* camera = new CameraComponent(CameraType::PERSPECTIVE);
        camera->set_perspective(fov, aspect, near_plane, far_plane);
        return camera;
    }

    CameraComponent * CameraComponent::
    create_orthographic(float size, float aspect, float near_plane, float far_plane) {
        auto* camera = new CameraComponent(CameraType::ORTHOGRAPHIC);
        float half_width = size * aspect * 0.5f;
        float half_height = size * 0.5f;
        camera->set_orthographic(-half_width, half_width, -half_height, half_height, near_plane, far_plane);
        return camera;
    }

    void CameraComponent::update_camera_vectors() {
        // Calculate front vector from yaw and pitch
        glm::vec3 front;
        front.x = cos(glm::radians(camera_data_.yaw)) * cos(glm::radians(camera_data_.pitch));
        front.y = sin(glm::radians(camera_data_.pitch));
        front.z = sin(glm::radians(camera_data_.yaw)) * cos(glm::radians(camera_data_.pitch));
        front_ = glm::normalize(front);

        // Calculate right and up vectors
        right_ = glm::normalize(glm::cross(front_, world_up_));
        up_ = glm::normalize(glm::cross(right_, front_));
    }

    void CameraComponent::update_view_matrix() const {
        if (!get_owner()) {
            view_matrix_ = glm::mat4(1.0f);
            view_dirty_ = false;
            return;
        }

        auto* transform = get_owner()->get_component<TransformComponent>();
        if (!transform) {
            view_matrix_ = glm::mat4(1.0f);
            view_dirty_ = false;
            return;
        }

        glm::vec3 position = transform->get_position();
        view_matrix_ = glm::lookAt(position, position + front_, up_);
        view_dirty_ = false;
    }

    void CameraComponent::update_projection_matrix() const {
        if (camera_type_ == CameraType::PERSPECTIVE) {
            projection_matrix_ = glm::perspective(glm::radians(fov_), aspect_ratio_, near_plane_, far_plane_);
        } else {
            projection_matrix_ = glm::ortho(ortho_left_, ortho_right_, ortho_bottom_, ortho_top_, near_plane_, far_plane_);
        }
        projection_dirty_ = false;
    }

    void CameraComponent::update_orthographic_bounds() {
        float half_width = ortho_size_ * aspect_ratio_ * 0.5f;
        float half_height = ortho_size_ * 0.5f;
        ortho_left_ = -half_width;
        ortho_right_ = half_width;
        ortho_bottom_ = -half_height;
        ortho_top_ = half_height;
    }
}
