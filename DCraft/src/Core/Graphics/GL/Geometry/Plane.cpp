#include "DCraft/Graphics/Primitives/Plane.h"

#include <glm/gtc/type_ptr.hpp>

namespace DCraft 
{
Plane::Plane()
{
    std::vector<Vertex> vertices_data;
    std::vector<unsigned int> indices_data;
    std::vector<Texture> texture_data = {};

    // Define the cube vertex data
    for (size_t i = 0; i < 4; ++i)
    {
        Vertex v;
        v.position = glm::vec3(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
        v.color = glm::vec3(colors[i * 3], colors[i * 3 + 1], colors[i * 3 + 2]);
        v.normal = glm::vec3(0.0f);
        vertices_data.push_back(v);
    }

    // Convert element array to vector
    for (size_t i = 0; i < std::size(plane_elements); ++i)
    {
        indices_data.push_back(static_cast<unsigned int>(plane_elements[i]));
    }

    Mesh cubeMesh(vertices_data, indices_data, texture_data);

    get_model().add_mesh(cubeMesh);
	
	get_model().transform = glm::mat4(1.0f);
    world_matrix_ = glm::mat4(1.0f);

    update_model_matrix();
}

void Plane::draw_self(const glm::mat4& view, const glm::mat4& projection, uint32_t shader_program)
{
    glm::mat4 mvp = projection * view * get_world_matrix();

    unsigned int mvpLoc = glGetUniformLocation(shader_program, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    if (!get_model().meshes.empty())
    {
	    for (auto& mesh: get_model().meshes)
	    {
            mesh.draw_mesh();
	    }
    }

}


void Plane::update(float dt)
{
    // Get the current rotation angle in degrees (convert from radians)
    float current_degrees = get_rotation_angle() * 180.0f / glm::pi<float>();
    glm::vec3 current_scale = get_current_scale();

    // Increment by 1 degree
    current_degrees += 1.0f;
    current_scale += 0.0005f;

    // Set the new rotation (this will convert back to radians internally)
    //set_position(get_position() + glm::vec3(0.05, 0.0,0.0));
    set_rotation(current_degrees, glm::vec3(1.0, 1.0, 0.0));
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




