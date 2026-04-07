//
// Created by denzel on 08/08/2025.
//
#pragma once
#include <glm/glm.hpp>

#include "TransformComponent.h"
#include "DCraft/Structs/Component.h"
#include "DCraft/Structs/Entity.h"

namespace DCraft {
    enum class CameraType {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    struct CameraData {
        float yaw = -90.0f;
        float pitch = 0.0f;
        float sensitivity = 0.1f;
        float zoom = 45.0f;
    };

    class CameraComponent : public Component {
    public:
        CameraComponent(CameraType type = CameraType::PERSPECTIVE) : camera_type_(type) {
            update_camera_vectors();
        }

        // Camera type management
        CameraType get_camera_type() const { return camera_type_; }

        void set_camera_type(CameraType type) {
            camera_type_ = type;
            projection_dirty_ = true;
        }

        // Perspective camera setup
        void set_perspective(float fov, float aspect, float near_plane, float far_plane) {
            camera_type_ = CameraType::PERSPECTIVE;
            fov_ = fov;
            aspect_ratio_ = aspect;
            near_plane_ = near_plane;
            far_plane_ = far_plane;
            projection_dirty_ = true;
        }

        // Orthographic camera setup
        void set_orthographic(float left, float right, float bottom, float top, float near_plane, float far_plane) {
            camera_type_ = CameraType::ORTHOGRAPHIC;
            ortho_left_ = left;
            ortho_right_ = right;
            ortho_bottom_ = bottom;
            ortho_top_ = top;
            near_plane_ = near_plane;
            far_plane_ = far_plane;
            projection_dirty_ = true;
        }

        // Matrix getters
        glm::mat4 get_view_matrix() const {
            if (view_dirty_) {
                update_view_matrix();
            }
            return view_matrix_;
        }

        glm::mat4 get_projection_matrix() const {
            if (projection_dirty_) {
                update_projection_matrix();
            }
            return projection_matrix_;
        }

        // Property setters/getters
        void set_fov(float fov) {
            fov_ = fov;
            projection_dirty_ = true;
        }

        float get_fov() const { return fov_; }

        void set_aspect_ratio(float aspect) {
            aspect_ratio_ = aspect;
            projection_dirty_ = true;
        }

        float get_aspect_ratio() const { return aspect_ratio_; }

        void set_clip_planes(float near_plane, float far_plane) {
            near_plane_ = near_plane;
            far_plane_ = far_plane;
            projection_dirty_ = true;
        }

        float get_near_plane() const { return near_plane_; }
        float get_far_plane() const { return far_plane_; }

        void set_yaw(float yaw) { 
            camera_data_.yaw = yaw; 
            update_camera_vectors();
            view_dirty_ = true;
        }
    
        void set_pitch(float pitch) { 
            camera_data_.pitch = glm::clamp(pitch, -89.0f, 89.0f);
            update_camera_vectors();
            view_dirty_ = true;
        }
    
        // Getters for PlayerController to read current values
        float get_yaw() const { return camera_data_.yaw; }
        float get_pitch() const { return camera_data_.pitch; }
    
        // Mouse sensitivity (camera-related)
        void set_mouse_sensitivity(float sensitivity) { 
            camera_data_.sensitivity = sensitivity; 
        }
        float get_mouse_sensitivity() const { return camera_data_.sensitivity; }


        // Camera vectors
        glm::vec3 get_front_vector() const { return front_; }
        glm::vec3 get_up_vector() const { return up_; }
        glm::vec3 get_right_vector() const { return right_; }

        // Target and orientation
        void look_at(const glm::vec3 &target) {
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

        void set_target(const glm::vec3& target) { 
            target_ = target;
            look_at(target);
        }
        glm::vec3 get_target() const { return target_; }

        void invalidate_view() {
            view_dirty_ = true;
        }

        // Factory methods
        static CameraComponent* create_perspective(float fov = 45.0f, float aspect = 16.0f/9.0f, 
                                                  float near_plane = 0.1f, float far_plane = 100.0f) {
            auto* camera = new CameraComponent(CameraType::PERSPECTIVE);
            camera->set_perspective(fov, aspect, near_plane, far_plane);
            return camera;
        }

        static CameraComponent* create_orthographic(float size = 10.0f, float aspect = 16.0f/9.0f,
                                                   float near_plane = 0.1f, float far_plane = 100.0f) {
            auto* camera = new CameraComponent(CameraType::ORTHOGRAPHIC);
            float half_width = size * aspect * 0.5f;
            float half_height = size * 0.5f;
            camera->set_orthographic(-half_width, half_width, -half_height, half_height, near_plane, far_plane);
            return camera;
        }
    private:
        // Camera type
        CameraType camera_type_ = CameraType::PERSPECTIVE;

        // Perspective camera properties
        float fov_ = 45.0f;
        float aspect_ratio_ = 16.0f / 9.0f;
        float near_plane_ = 0.1f;
        float far_plane_ = 100.0f;

        // Orthographic camera properties
        float ortho_size_ = 10.0f;
        float ortho_left_ = -10.0f;
        float ortho_right_ = 10.0f;
        float ortho_bottom_ = -10.0f;
        float ortho_top_ = 10.0f;

        // Camera vectors and orientation
        glm::vec3 front_ = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right_ = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 world_up_ = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 target_ = glm::vec3(0.0f);

        CameraData camera_data_;

        // Cached matrices
        mutable glm::mat4 view_matrix_ = glm::mat4(1.0f);
        mutable glm::mat4 projection_matrix_ = glm::mat4(1.0f);
        mutable bool view_dirty_ = true;
        mutable bool projection_dirty_ = true;

    private:
        void update_camera_vectors() {
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

        void update_view_matrix() const {
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

        void update_projection_matrix() const {
            if (camera_type_ == CameraType::PERSPECTIVE) {
                projection_matrix_ = glm::perspective(glm::radians(fov_), aspect_ratio_, near_plane_, far_plane_);
            } else {
                projection_matrix_ = glm::ortho(ortho_left_, ortho_right_, ortho_bottom_, ortho_top_, near_plane_, far_plane_);
            }
            projection_dirty_ = false;
        }

        void update_orthographic_bounds() {
            float half_width = ortho_size_ * aspect_ratio_ * 0.5f;
            float half_height = ortho_size_ * 0.5f;
            ortho_left_ = -half_width;
            ortho_right_ = half_width;
            ortho_bottom_ = -half_height;
            ortho_top_ = half_height;
        }
    };
}
