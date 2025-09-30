#pragma once
#include <string>
#include <glm/glm.hpp>

namespace hellfire {
    class Scene;
    using EntityID = uint32_t;

    class PerspectiveCamera {
    public:
        static EntityID create(
            Scene* scene,
            const std::string& name, 
            float fov = 70.0f, 
            float aspect = 16.0f/9.0f, 
            float near_plane = 0.1f,
            float far_plane = 1000.0f, 
            const glm::vec3& position = glm::vec3(0.0f)
        );
        
        static EntityID create_looking_at(
            Scene* scene,
            const std::string& name, 
            const glm::vec3& position,
            const glm::vec3& target, 
            float fov = 70.0f, 
            float aspect = 16.0f/9.0f, 
            float near_plane = 0.1f, 
            float far_plane = 1000.0f
        );
    };

    class OrthographicCamera {
    public:
        static EntityID create(
            Scene* scene,
            const std::string& name, 
            float size, 
            float aspect, 
            float near_plane = -1.0f,
            float far_plane = 1.0f, 
            const glm::vec3& position = glm::vec3(0.0f)
        );
        
        static EntityID create_with_bounds(
            Scene* scene,
            const std::string& name, 
            float left, 
            float right, 
            float bottom,
            float top, 
            float near_plane = -1.0f, 
            float far_plane = 1.0f, 
            const glm::vec3& position = glm::vec3(0.0f)
        );
    };
}

