#pragma once
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "hellfire/ecs/Entity.h"


struct Vertex;

namespace hellfire
{
    class Scene;
    class Mesh;
    class Material;

    class Quad 
    {
    public:
        struct Config {
            glm::vec3 color = glm::vec3(1.0f);
            std::shared_ptr<Material> material = nullptr;
            glm::vec3 position = glm::vec3(0.0f);
            glm::vec3 rotation = glm::vec3(0.0f);
            glm::vec3 scale = glm::vec3(1.0f);
        };

        static EntityID create(Scene* scene, const std::string& name, const Config& config = Config{});
        static std::shared_ptr<Mesh> create_mesh();
        static void get_quad_data(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, 
                                 const glm::vec3& color = glm::vec3(1.0f));
        
    private:
        static const std::vector<float> vertices_;
        static const std::vector<float> uvs_;  
        static const std::vector<unsigned int> indices_;

        Quad() = delete;
    };

}
