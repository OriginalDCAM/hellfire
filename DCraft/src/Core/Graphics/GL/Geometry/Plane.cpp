#include "DCraft/Graphics/Primitives/Plane.h"

#include <glm/gtc/type_ptr.hpp>

namespace DCraft 
{
Plane::Plane()
{
    std::vector<Vertex> vertices_data;
    std::vector<unsigned int> indices_data;
    std::vector<Texture*> texture_data = {};

    // Define the cube vertex data
    for (size_t i = 0; i < 4; ++i)
    {
        Vertex v;
        v.position = glm::vec3(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
        v.color = glm::vec3(colors[i * 3], colors[i * 3 + 1], colors[i * 3 + 2]);
        v.normal = glm::vec3(0.0f);
        v.texCoords = glm::vec2(uvs[i * 2], uvs[i * 2 + 1]);
        vertices_data.push_back(v);
    }

    // Convert element array to vector
    for (size_t i = 0; i < std::size(plane_elements); ++i)
    {
        indices_data.push_back(static_cast<unsigned int>(plane_elements[i]));
    }

    Mesh planeMesh(vertices_data, indices_data);
    add_mesh(planeMesh);
	
    update_world_matrix();
}

void Plane::update(float dt)
{

}

void Plane::set_vertices()
{
}

void Plane::set_colors()
{
	
}

void Plane::set_uvs()
{
	
}

}




