#include "CameraFactory.h"
#include "hellfire/scene/Scene.h"
#include "hellfire/ecs/CameraComponent.h"
#include "hellfire/ecs/TransformComponent.h"

namespace hellfire {
    EntityID PerspectiveCamera::create(Scene* scene, const std::string& name, 
                                       float fov, float aspect, float near_plane,
                                       float far_plane, const glm::vec3& position) {
        EntityID id = scene->create_entity(name);
        Entity* entity = scene->get_entity(id);
        
        auto* camera = entity->add_component<CameraComponent>(CameraType::PERSPECTIVE);
        camera->set_perspective(fov, aspect, near_plane, far_plane);
        
        entity->transform()->set_position(position.x, position.y, position.z);
        
        return id;
    }

    EntityID PerspectiveCamera::create_looking_at(Scene* scene, const std::string& name, 
                                                  const glm::vec3& position,
                                                  const glm::vec3& target, 
                                                  float fov, float aspect, 
                                                  float near_plane, float far_plane) {
        EntityID id = create(scene, name, fov, aspect, near_plane, far_plane, position);
        Entity* entity = scene->get_entity(id);
        entity->get_component<CameraComponent>()->look_at(target);
        return id;
    }

    EntityID OrthographicCamera::create(Scene* scene, const std::string& name, 
                                       float size, float aspect, float near_plane,
                                       float far_plane, const glm::vec3& position) {
        EntityID id = scene->create_entity(name);
        Entity* entity = scene->get_entity(id);
        
        auto* camera = entity->add_component<CameraComponent>(CameraType::ORTHOGRAPHIC);
        
        float half_width = size * aspect * 0.5f;
        float half_height = size * 0.5f;
        camera->set_orthographic(-half_width, half_width, -half_height, half_height, near_plane, far_plane);
        
        entity->transform()->set_position(position.x, position.y, position.z);
        
        return id;
    }

    EntityID OrthographicCamera::create_with_bounds(Scene* scene, const std::string& name, 
                                                    float left, float right, float bottom,
                                                    float top, float near_plane, float far_plane, 
                                                    const glm::vec3& position) {
        EntityID id = scene->create_entity(name);
        Entity* entity = scene->get_entity(id);
        
        auto* camera = entity->add_component<CameraComponent>(CameraType::ORTHOGRAPHIC);
        camera->set_orthographic(left, right, bottom, top, near_plane, far_plane);
        
        entity->transform()->set_position(position.x, position.y, position.z);
        
        return id;
    }
}