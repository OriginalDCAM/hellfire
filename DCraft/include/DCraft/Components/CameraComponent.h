//
// Created by denzel on 08/08/2025.
//
#pragma once

#include "DCraft/Structs/Component.h"
#include "TransformComponent.h"

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
        CameraComponent(CameraType type = CameraType::PERSPECTIVE);

        // Camera type management
        CameraType get_camera_type() const { return camera_type_; }

        void set_camera_type(CameraType type);

        // Perspective camera setup
        void set_perspective(float fov, float aspect, float near_plane, float far_plane);

        // Orthographic camera setup
        void set_orthographic(float left, float right, float bottom, float top, float near_plane, float far_plane);

        // Matrix getters
        glm::mat4 get_view_matrix() const;

        glm::mat4 get_projection_matrix() const;

        // Property setters/getters
        void set_fov(float fov);

        float get_fov() const { return fov_; }

        void set_aspect_ratio(float aspect);

        float get_aspect_ratio() const { return aspect_ratio_; }

        void set_clip_planes(float near_plane, float far_plane);

        float get_near_plane() const { return near_plane_; }
        float get_far_plane() const { return far_plane_; }

        void set_yaw(float yaw);

        void set_pitch(float pitch);

        // Getters for PlayerController to read current values
        float get_yaw() const { return camera_data_.yaw; }
        float get_pitch() const { return camera_data_.pitch; }
    
        // Mouse sensitivity (camera-related)
        void set_mouse_sensitivity(float sensitivity);

        float get_mouse_sensitivity() const { return camera_data_.sensitivity; }


        // Camera vectors
        glm::vec3 get_front_vector() const { return front_; }
        glm::vec3 get_up_vector() const { return up_; }
        glm::vec3 get_right_vector() const { return right_; }

        // Target and orientation
        void look_at(const glm::vec3 &target);

        void set_target(const glm::vec3& target);

        glm::vec3 get_target() const { return target_; }

        void invalidate_view() const {
            view_dirty_ = true;
        }

        // Factory methods
        static CameraComponent* create_perspective(float fov = 45.0f, float aspect = 16.0f/9.0f, 
                                                  float near_plane = 0.1f, float far_plane = 100.0f);

        static CameraComponent* create_orthographic(float size = 10.0f, float aspect = 16.0f/9.0f,
                                                    float near_plane = 0.1f, float far_plane = 100.0f);

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

        void update_camera_vectors();

        void update_view_matrix() const;

        void update_projection_matrix() const;

        void update_orthographic_bounds();
    };
}
