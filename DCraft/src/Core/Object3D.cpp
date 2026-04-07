#include "DCraft/Structs/Object3D.h"

namespace DCraft {
    void Object3D::set_rotation_matrix(const glm::mat4 &rotation_matrix) {
        rotation_matrix_ = rotation_matrix;
        use_rotation_matrix_ = true;
        update_model_matrix();
    }


    void Object3D::set_translation_matrix(const glm::mat4 &translation_matrix) {
        translation_matrix_ = translation_matrix;
        use_translation_matrix_ = true;
        update_model_matrix();
    }


    void Object3D::set_scale_matrix(const glm::mat4 &scale_matrix) {
        scale_matrix_ = scale_matrix;
        use_scale_matrix_ = true;
        update_model_matrix();
    }

    void Object3D::update_model_matrix() const {
        model_ptr_->transform = glm::mat4(1.0f);

        if (use_translation_matrix_) {
            model_ptr_->transform = model_ptr_->transform * translation_matrix_;
        } else {
            model_ptr_->transform = glm::translate(model_ptr_->transform, position_);
        }
        
        if (use_rotation_matrix_) {
            model_ptr_->transform = model_ptr_->transform * rotation_matrix_;
        } else if (glm::length(rotation_axis_) > 0.0001f) {
            glm::vec3 normalized_axis = glm::normalize(rotation_axis_);
            model_ptr_->transform = glm::rotate(model_ptr_->transform, rotation_angle_, normalized_axis);
        }

        if (use_scale_matrix_) {
            model_ptr_->transform = model_ptr_->transform * scale_matrix_;
        } else {
            model_ptr_->transform = glm::scale(model_ptr_->transform, scale_);
        }
    }
}
