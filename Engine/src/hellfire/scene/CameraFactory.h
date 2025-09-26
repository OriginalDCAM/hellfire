#pragma once

#include "hellfire/ecs/CameraComponent.h"

namespace hellfire {
        class PerspectiveCamera {
    public:
        static Entity* create(const std::string& name = "PerspectiveCamera",
                             float fov = 45.0f,
                             float aspect = 16.0f / 9.0f,
                             float near_plane = 0.1f,
                             float far_plane = 100.0f,
                             const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f));

        // Convenience method to create camera looking at target
        static Entity* create_looking_at(const std::string& name,
                                        const glm::vec3& position,
                                        const glm::vec3& target,
                                        float fov = 45.0f,
                                        float aspect = 16.0f / 9.0f,
                                        float near_plane = 0.1f,
                                        float far_plane = 100.0f);
        };

    class OrthographicCamera {
    public:
        static Entity* create(const std::string& name = "OrthographicCamera",
                             float size = 10.0f,
                             float aspect = 16.0f / 9.0f,
                             float near_plane = 0.1f,
                             float far_plane = 100.0f,
                             const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f));

        static Entity* create_with_bounds(const std::string& name,
                                          float left, float right, float bottom, float top,
                                          float near_plane = 0.1f, float far_plane = 100.0f,
                                          const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f));
    };
    
}

