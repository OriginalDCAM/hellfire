#include "DCraft/Structs/Object3D.h"

namespace DCraft {
    Object3D * Object3D::find_object_by_name(const std::string &name) {
        if (get_name() == name) return this;

        for (auto* child : children_) {
            Object3D* found = child->find_object_by_name(name);
            if (found) {
                return found;
            }
        }
        return nullptr;
    }

    

    void Object3D::match_orientation(const Object3D &other) {
        transform_.match_orientation(other.get_transform());
    }

    void Object3D::look_at(const glm::vec3 &target, const glm::vec3 &up) {
        transform_.look_at(target, up);
    }

    glm::vec3 Object3D::get_default_front() const {
        // Towards positive z axis
        return glm::vec3(0.0f, 0.0f, 1.0f);
    }


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
