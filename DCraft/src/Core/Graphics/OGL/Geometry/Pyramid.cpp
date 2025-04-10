#include "DCraft/Graphics/Primitives/Pyramid.h"

#include <glm/gtc/type_ptr.hpp>

namespace DCraft
{
    Pyramid::Pyramid(const std::string& name) : Shape3D(name)
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
            vertices_data.push_back(v);
        }

        // Convert element array to vector
        for (size_t i = 0; i < std::size(pyramid_elements); ++i)
        {
            indices_data.push_back(static_cast<unsigned int>(pyramid_elements[i]));
        }

        Mesh* pyramidMesh = new Mesh(vertices_data, indices_data);
        set_mesh(pyramidMesh);

        update_world_matrix();
    }

    void Pyramid::update(float dt)
    {
        // Get the current rotation angle in degrees (convert from radians)
        float current_degrees = get_rotation_angle() * 180.0f / glm::pi<float>();
        glm::vec3 current_scale = get_scale();

        // Increment by 1 degree
        current_degrees += 1.0f;
        current_scale += 0.0005f;

        // Set the new rotation (this will convert back to radians internally)
        //set_position(get_position() + glm::vec3(0.05, 0.0,0.0));
        set_rotation(current_degrees, glm::vec3(1.0, 1.0, 0.0));
    }

    void Pyramid::set_vertices()
    {
    }

    void Pyramid::set_colors()
    {

    }

    void Pyramid::set_uvs()
    {

    }
}
