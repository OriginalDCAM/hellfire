//
// Created by denzel on 06/04/2025.
//

#include "hellfire/graphics/Transform3D.h"

#include <glm/gtc/quaternion.hpp>

namespace hellfire {
    void Transform3D::look_at(const glm::vec3 &target, const glm::vec3 &up) {
        // Calculate Direction vectors
        glm::vec3 direction = glm::normalize(target - get_position());
        glm::vec3 right = glm::normalize(glm::cross(direction, up));
        glm::vec3 adjusted_up = glm::normalize(glm::cross(right, direction));

        // Create rotation matrix
        glm::mat4 rotation(1.0f);
        rotation[0] = glm::vec4(right, 0.0f);
        rotation[1] = glm::vec4(adjusted_up, 0.0f);
        rotation[2] = glm::vec4(direction, 0.0f);

        rotation_matrix_ = rotation;
        use_rotation_matrix_ = true;
        update_local_matrix();
    }

    void Transform3D::set_rotation(const glm::vec3 &angles) {
        rotation_x_ = glm::radians(angles.x);
        rotation_y_ = glm::radians(angles.y);
        rotation_z_ = glm::radians(angles.z);
        rotation_in_degrees_ = glm::vec3(glm::degrees(rotation_x_), glm::degrees(rotation_y_), glm::degrees(rotation_z_));
        use_euler_angles_ = true;
        use_rotation_matrix_ = false;
        update_local_matrix();
    }

    void Transform3D::set_rotation_quaternion(const glm::quat &q) {
        // Convert quaternion to Euler angles (in radians)
        glm::vec3 euler = glm::eulerAngles(q);

        // Convert to degrees if your engine uses degrees
        glm::vec3 euler_degrees = glm::degrees(euler);

        // Set rotation using your existing method
        set_rotation(euler_degrees);
    }

    const glm::vec3& Transform3D::get_rotation() const {
        return rotation_in_degrees_;

    }

    void Transform3D::extract_euler_angles_(const glm::mat4 &rotation_matrix, float &x, float &y,
        float &z) {
        // Extract rotation part (3x3 upper-left submatrix)
        glm::mat3 rot3(rotation_matrix);
    
        // Handle gimbal lock case
        if (std::abs(rot3[0][2]) > 0.998f) {
            // Gimbal lock - singularity at pitch = +/-90 degrees
            y = atan2f(-rot3[2][0], rot3[0][0]) * (rot3[0][2] > 0 ? -1.0f : 1.0f);
            x = (rot3[0][2] > 0 ? -1.0f : 1.0f) * glm::half_pi<float>();
            z = 0.0f;
        } else {
            // Standard case
            y = atan2f(rot3[0][2], rot3[2][2]);
            x = -asinf(rot3[1][2]);
            z = atan2f(rot3[1][0], rot3[1][1]);
        }
    }

    void Transform3D::match_orientation(const Transform3D &other) {
        rotation_matrix_ = other.get_rotation_matrix();
        use_rotation_matrix_ = true;
        update_local_matrix();
    }
}
