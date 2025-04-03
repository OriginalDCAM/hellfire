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

    Mesh planeMesh(vertices_data, indices_data, texture_data);

    get_model()->add_mesh(planeMesh);
	
	get_model()->transform = glm::mat4(1.0f);
    world_matrix_ = glm::mat4(1.0f);

    update_model_matrix();
}

void Plane::draw_self(const glm::mat4& view, const glm::mat4& projection, uint32_t shader_program)
{
    glm::mat4 mvp = projection * view * get_world_matrix();

    unsigned int mvpLoc = glGetUniformLocation(shader_program, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    if (!get_model()->meshes.empty())
    {
	    for (auto& mesh: get_model()->meshes)
	    {
            mesh.draw_mesh(shader_program);
	    }
    }

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




