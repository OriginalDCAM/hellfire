//
// Created by denzel on 22/08/2025.
//

#include "CameraFactory.h"

#include "../ecs/Entity.h"

namespace hellfire {
    Entity * PerspectiveCamera::create(const std::string &name, float fov, float aspect, float near_plane,
        float far_plane, const glm::vec3 &position) {
            
        Entity* entity = new Entity(name);
        auto* camera = entity->add_component<CameraComponent>(CameraType::PERSPECTIVE);
        camera->set_perspective(fov, aspect, near_plane, far_plane);
            
        entity->transform()->set_position(position.x, position.y, position.z);
            
        return entity;
    }

    Entity * PerspectiveCamera::create_looking_at(const std::string &name, const glm::vec3 &position,
        const glm::vec3 &target, float fov, float aspect, float near_plane, float far_plane) {
        Entity* entity = create(name, fov, aspect, near_plane, far_plane, position);
        entity->get_component<CameraComponent>()->look_at(target);
        return entity;
    }

    Entity * OrthographicCamera::create(const std::string &name, float size, float aspect, float near_plane,
        float far_plane, const glm::vec3 &position) {
            
        Entity* entity = new Entity(name);
        auto* camera = entity->add_component<CameraComponent>(CameraType::ORTHOGRAPHIC);
            
        float half_width = size * aspect * 0.5f;
        float half_height = size * 0.5f;
        camera->set_orthographic(-half_width, half_width, -half_height, half_height, near_plane, far_plane);
            
        entity->transform()->set_position(position.x, position.y, position.z);
            
        return entity;
    }

    Entity * OrthographicCamera::create_with_bounds(const std::string &name, float left, float right, float bottom,
        float top, float near_plane, float far_plane, const glm::vec3 &position) {
        Entity* entity = new Entity(name);
        auto* camera = entity->add_component<CameraComponent>(CameraType::ORTHOGRAPHIC);
        camera->set_orthographic(left, right, bottom, top, near_plane, far_plane);
            
        entity->transform()->set_position(position.x, position.y, position.z);
            
        return entity;
    }
}
