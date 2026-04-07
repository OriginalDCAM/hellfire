//
// Created by denzel on 06/04/2025.
//

#include "DCraft/Structs/Transform3D.h"

#include <glm/gtc/quaternion.hpp>

namespace DCraft {
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

    void Transform3D::set_rotation(glm::vec3 &angles) {
        rotation_x_ = glm::radians(angles.x);
        rotation_y_ = glm::radians(angles.y);
        rotation_z_ = glm::radians(angles.z);
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

    void Transform3D::match_orientation(const Transform3D &other) {
        rotation_matrix_ = other.get_rotation_matrix();
        use_rotation_matrix_ = true;
        update_local_matrix();
    }
}
