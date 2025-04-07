#include "DCraft/Structs/Object3D.h"

namespace DCraft {
    const glm::vec3 Object3D::get_world_position() const {
        glm::mat4 world_matrix = get_world_matrix();

        return glm::vec3(world_matrix[3][0], world_matrix[3][1], world_matrix[3][2]);
    }

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
