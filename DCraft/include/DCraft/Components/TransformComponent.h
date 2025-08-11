//
// Created by denzel on 07/08/2025.
//
#pragma once
#include "DCraft/Structs/Component.h"
#include <glm/glm.hpp>

#include "DCraft/Structs/Transform3D.h"

namespace DCraft {
    class Transform3D;
}

namespace DCraft {
    class TransformComponent : public Component {
    public:
        TransformComponent() = default;
        const glm::vec3& get_position() const { return transform_.get_position(); }
        void set_position(float x, float y, float z) { transform_.set_position(x, y, z); }
        void set_position(const glm::vec3& position) { transform_.set_position(position); }

        const glm::vec3& get_rotation() { return transform_.get_rotation(); }
        void set_rotation(const glm::vec3& eulers) { transform_.set_rotation(eulers); }
        void set_rotation(float x, float y, float z) { transform_.set_rotation(glm::vec3(x, y, z)); }

        
        const glm::vec3& get_scale() const { return transform_.get_scale(); }
        void set_scale(float x, float y, float z) { transform_.set_scale(glm::vec3(x, y, z)); }
        void set_scale(float value) { transform_.set_scale(glm::vec3(value, value, value)); }
        void set_scale(glm::vec3& scale) { transform_.set_scale(scale); }
        

        const glm::vec3& get_world_position() const { return transform_.get_position(); }
    
        const glm::mat4& get_local_matrix() const { return transform_.get_local_matrix(); }
        const glm::mat4& get_world_matrix() const { return transform_.get_world_matrix(); }

        const glm::mat4& get_rotation_matrix() const { return transform_.get_rotation_matrix(); }

        // Delegate all other methods to the internal Transform3D
        void look_at(const glm::vec3& target, const glm::vec3& up = glm::vec3(0,1,0)) {
            transform_.look_at(target, up);
        }

        void update_world_matrix(const glm::mat4 *parent_world_matrix = nullptr) {
            transform_.update_world_matrix(parent_world_matrix);
        }
        
    private:
        Transform3D transform_;
    };
} // namespace DCraft
