#pragma once
#include <string>
#include <vector>

#include "DCraft/Graphics/Vertex.h"

namespace DCraft
{
    class Entity;

    class Cube 
    {

    public:
        static Entity* create(const std::string& name,  const glm::vec3& color = glm::vec3(1.0f));

        // Helper method to get cube geometry data
        static void get_cube_data(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, 
                                 const glm::vec3& color = glm::vec3(1.0f));
        
    private:
        static const std::vector<float> vertices_;
        static const std::vector<float> uvs_;  
        static const std::vector<unsigned int> indices_;

        Cube() = delete;
    };
}
