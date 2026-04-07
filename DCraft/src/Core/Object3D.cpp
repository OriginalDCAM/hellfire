#include "DCraft/Structs/Object3D.h"

namespace DCraft {
    void Object3D::set_rotation_matrix(const glm::mat4 &rotation_matrix) {
        transform_.set_rotation_matrix(rotation_matrix);
    }

    void Object3D::set_translation_matrix(const glm::mat4 &translation_matrix) {
        transform_.set_translation_matrix(translation_matrix);
    }

    void Object3D::set_scale_matrix(const glm::mat4 &scale_matrix) {
        transform_.set_scale_matrix(scale_matrix);
    }
}
