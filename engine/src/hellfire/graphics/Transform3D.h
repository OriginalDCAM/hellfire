#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <glm/detail/type_vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace hellfire {
    class Transform3D {
    public:
        Transform3D()
            : position_(0.0f, 0.0f, 0.0f)
              , scale_(1.0f, 1.0f, 1.0f)
              , rotation_axis_(0.0f, 0.0f, 0.0f)
              , rotation_angle_(0.0f)
              , local_matrix_(1.0f)
              , world_matrix_(1.0f)
              , rotation_matrix_(1.0f)
              , translation_matrix_(1.0f)
              , scale_matrix_(1.0f)
              , use_rotation_matrix_(false)
              , use_translation_matrix_(false)
              , use_scale_matrix_(false) {
        }

        // Position methods
        glm::vec3 &get_position() { return position_; }
        const glm::vec3 &get_position() const { return position_; }

        void set_position(const glm::vec3 &new_position) {
            position_ = new_position;
            update_local_matrix();
        }

        void set_position(const float x, const float y, const float z) {
            position_ = glm::vec3(x, y, z);
            update_local_matrix();
        }

        // Scale methods
        void set_scale(const glm::vec3 &new_scale) {
            scale_ = new_scale;
            update_local_matrix();
        }


        void look_at(const glm::vec3 &target, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));

        void match_orientation(const Transform3D &other);

        glm::vec3 get_scale() const { return scale_; }
        glm::vec3& get_scale() { return scale_; }

        void set_rotation(const glm::vec3 &angles);

        void set_rotation_quaternion(const glm::quat &q);

        float get_rotation_angle() const { return rotation_angle_; }
        const glm::vec3 &get_rotation_axis() const { return rotation_axis_; }

        // Matrix methods
        void set_rotation_matrix(const glm::mat4 &rotation_matrix) {
            rotation_matrix_ = rotation_matrix;
            use_rotation_matrix_ = true;
            update_local_matrix();
        }

        glm::mat4 get_rotation_matrix() const {
            return rotation_matrix_;
        }

        void set_translation_matrix(const glm::mat4 &translation_matrix) {
            translation_matrix_ = translation_matrix;
            use_translation_matrix_ = true;
            update_local_matrix();
        }

        glm::mat4 get_translation_matrix() const {
            return translation_matrix_;
        }

        glm::mat4 get_scale_matrix() const {
            return scale_matrix_;
        }

        void set_scale_matrix(const glm::mat4 &scale_matrix) {
            scale_matrix_ = scale_matrix;
            use_scale_matrix_ = true;
            update_local_matrix();
        }

        const glm::mat4 &get_local_matrix() const { return local_matrix_; }
        const glm::mat4 &get_world_matrix() const { return world_matrix_; }


        void update_local_matrix() {
            local_matrix_ = glm::mat4(1.0f);

            if (use_translation_matrix_) {
                local_matrix_ = local_matrix_ * translation_matrix_;
            } else {
                local_matrix_ = glm::translate(local_matrix_, position_);
            }

            if (use_rotation_matrix_) {
                local_matrix_ = local_matrix_ * rotation_matrix_;
                
                // Extract and store euler angles for consistency
                extract_euler_angles_(rotation_matrix_, rotation_x_, rotation_y_, rotation_z_);
            } else if (use_euler_angles_) {
                glm::mat4 rot_x = glm::rotate(glm::mat4(1.0f), rotation_x_, glm::vec3(1, 0, 0));
                glm::mat4 rot_y = glm::rotate(glm::mat4(1.0f), rotation_y_, glm::vec3(0, 1, 0));
                glm::mat4 rot_z = glm::rotate(glm::mat4(1.0f), rotation_z_, glm::vec3(0, 0, 1));
                // Combine rotations
                glm::mat4 rotation_matrix = rot_z * rot_y * rot_x;

                // Apply to local matrix
                local_matrix_ = local_matrix_ * rotation_matrix;
            } else if (glm::length(rotation_axis_) > 0.0001f) {
                glm::vec3 normalized_axis = glm::normalize(rotation_axis_);
                local_matrix_ = glm::rotate(local_matrix_, rotation_angle_, normalized_axis);
            }

            if (use_scale_matrix_) {
                local_matrix_ = local_matrix_ * scale_matrix_;
            } else {
                local_matrix_ = glm::scale(local_matrix_, scale_);
            }
        }

        void update_world_matrix(const glm::mat4& parent_world_matrix) {
                world_matrix_ = parent_world_matrix * local_matrix_;
        }

        // Reset matrices to identity - useful for initialization
        void reset_to_identity() {
            local_matrix_ = glm::mat4(1.0f);
            world_matrix_ = glm::mat4(1.0f);
            // Reset transform components
            position_ = glm::vec3(0.0f);
            scale_ = glm::vec3(1.0f);
            rotation_axis_ = glm::vec3(0.0f);
            rotation_angle_ = 0.0f;
            // Reset custom matrices
            rotation_matrix_ = glm::mat4(1.0f);
            translation_matrix_ = glm::mat4(1.0f);
            scale_matrix_ = glm::mat4(1.0f);
            // Reset flags
            use_rotation_matrix_ = false;
            use_translation_matrix_ = false;
            use_scale_matrix_ = false;
        }

        const glm::vec3& get_rotation() const;

    private:
        glm::vec3 position_;
        glm::vec3 scale_;
        glm::vec3 rotation_axis_;
        float rotation_angle_;

        glm::mat4 local_matrix_; // Local transform matrix
        glm::mat4 world_matrix_; // World transform matrix

        // Transform matrices for direct manipulation
        glm::mat4 rotation_matrix_;
        glm::mat4 translation_matrix_;
        glm::mat4 scale_matrix_;

        bool use_rotation_matrix_;
        bool use_translation_matrix_;
        bool use_scale_matrix_;
        float rotation_x_ = 0.0f;
        float rotation_y_ = 0.0f;
        float rotation_z_ = 0.0f;
        glm::vec3 rotation_in_degrees_ = glm::vec3(0.0f);
        bool use_euler_angles_ = false;

        void extract_euler_angles_(const glm::mat4 &rotation_matrix, float &x, float &y, float &z);
    };
}
