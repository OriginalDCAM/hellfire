#pragma once

#include "DCraft/Components/CameraComponent.h"

namespace DCraft {
        class PerspectiveCamera {
    public:
        static Entity* create(const std::string& name = "PerspectiveCamera",
                             float fov = 45.0f,
                             float aspect = 16.0f / 9.0f,
                             float near_plane = 0.1f,
                             float far_plane = 100.0f,
                             const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f)) {
            
            Entity* entity = new Entity(name);
            auto* camera = entity->add_component<CameraComponent>(CameraType::PERSPECTIVE);
            camera->set_perspective(fov, aspect, near_plane, far_plane);
            
            entity->transform()->set_position(position.x, position.y, position.z);
            
            return entity;
        }
        
        // Convenience method to create camera looking at target
        static Entity* create_looking_at(const std::string& name,
                                        const glm::vec3& position,
                                        const glm::vec3& target,
                                        float fov = 45.0f,
                                        float aspect = 16.0f / 9.0f,
                                        float near_plane = 0.1f,
                                        float far_plane = 100.0f) {
            Entity* entity = create(name, fov, aspect, near_plane, far_plane, position);
            entity->get_component<CameraComponent>()->look_at(target);
            return entity;
        }
    };

    class OrthographicCamera {
    public:
        static Entity* create(const std::string& name = "OrthographicCamera",
                             float size = 10.0f,
                             float aspect = 16.0f / 9.0f,
                             float near_plane = 0.1f,
                             float far_plane = 100.0f,
                             const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f)) {
            
            Entity* entity = new Entity(name);
            auto* camera = entity->add_component<CameraComponent>(CameraType::ORTHOGRAPHIC);
            
            float half_width = size * aspect * 0.5f;
            float half_height = size * 0.5f;
            camera->set_orthographic(-half_width, half_width, -half_height, half_height, near_plane, far_plane);
            
            entity->transform()->set_position(position.x, position.y, position.z);
            
            return entity;
        }
        
        static Entity* create_with_bounds(const std::string& name,
                                         float left, float right, float bottom, float top,
                                         float near_plane = 0.1f, float far_plane = 100.0f,
                                         const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f)) {
            Entity* entity = new Entity(name);
            auto* camera = entity->add_component<CameraComponent>(CameraType::ORTHOGRAPHIC);
            camera->set_orthographic(left, right, bottom, top, near_plane, far_plane);
            
            entity->transform()->set_position(position.x, position.y, position.z);
            
            return entity;
        }
    };
    
}

