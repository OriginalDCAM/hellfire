#include "DCraft/Graphics/Primitives/Plane.h"

#include <glm/gtc/type_ptr.hpp>

namespace DCraft 
{
Plane::Plane(const std::string& name) : Shape3D(name)
{
    std::vector<Vertex> vertices_data;
    std::vector<unsigned int> indices_data;
    std::vector<Texture*> texture_data = {};

    glm::vec3 v0 = glm::vec3(vertices[0], vertices[1], vertices[2]);
    glm::vec3 v1 = glm::vec3(vertices[3], vertices[4], vertices[5]);
    glm::vec3 v2 = glm::vec3(vertices[6], vertices[7], vertices[8]);
    
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
    
    // Define the plane vertex data
    for (size_t i = 0; i < 4; ++i)
    {
        Vertex v;
        v.position = glm::vec3(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
        v.color = glm::vec3(colors[i * 3], colors[i * 3 + 1], colors[i * 3 + 2]);
        v.normal = normal; 
        v.texCoords = glm::vec2(uvs[i * 2], uvs[i * 2 + 1]);
        vertices_data.push_back(v);
    }

    // Convert element array to vector
    for (size_t i = 0; i < std::size(plane_elements); ++i)
    {
        indices_data.push_back(static_cast<unsigned int>(plane_elements[i]));
    }

    Mesh* planeMesh = new Mesh(vertices_data, indices_data);
    set_mesh(planeMesh);
	
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




