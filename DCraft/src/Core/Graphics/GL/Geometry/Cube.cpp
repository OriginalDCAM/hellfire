#include "Dcraft/Graphics/Primitives/Cube.h"

#include <iostream>
#include <glm/gtc/type_ptr.inl>

namespace DCraft
{

    Cube::Cube()
    {
        std::vector<Vertex> vertices_data;
        std::vector<unsigned int> indices_data;
        std::vector<Texture*> texture_data = {};

        // Create the cube's vertex data
        for (size_t i = 0; i < 24; ++i)
        {
            Vertex v;
            v.position = glm::vec3(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
            v.color = glm::vec3(colors[i * 3], colors[i * 3 + 1], colors[i * 3 + 2]);

            // Calculate the normals based on face
            // 0: front
            // 1: back
            // 2: top
            // 3: bottom
            // 4: right
            // 5: left
            int face = i / 4;

            switch (face) {
                case 0: v.normal = glm::vec3(0.0f, 0.0f, 1.0f); break;
                case 1: v.normal = glm::vec3(0.0f, 0.0f, -1.0); break;
                case 2: v.normal = glm::vec3(0.0f, 1.0f, 0.0f); break;
                case 3: v.normal = glm::vec3(0.0f, -1.0f, 0.0f); break;
                case 4: v.normal = glm::vec3(1.0f, 0.0f, 0.0f); break;
                case 5: v.normal = glm::vec3(-1.0f, 0.0f, 0.0f); break;
            }
            
            v.texCoords = glm::vec2(uvs[i * 2], uvs[i * 2 + 1]);
            vertices_data.push_back(v);
        }

        // Make sure to have enough memory to avoid having reallocations of memory 
        indices_data.reserve(std::size(cube_elements));

        // Convert element array to vector
        for (size_t i = 0; i < std::size(cube_elements); ++i)
        {
            indices_data.push_back(static_cast<unsigned int>(cube_elements[i]));
        }

        if (has_texture()) {
            texture_data.push_back(get_texture());
        }

        // Create mesh and add to the model
        Mesh cubeMesh(vertices_data, indices_data, texture_data);
        add_mesh(cubeMesh);

        // Initialize transformations
        update_world_matrix();
    }

    void Cube::update(float dt)
    {
        // Get the current rotation angle in degrees (convert from radians)
        // float current_degrees = get_rotation_angle() * 180.0f / glm::pi<float>();
        // current_degrees += 40.0f * dt;
        // float movement_speed = 2.0f;
        // float movement_amount = movement_speed * dt;
        // Set the new rotation (this will convert back to radians internally)
        // set_position(get_position() + glm::vec3(movement_amount, 0.0, 0.0));
        // set_rotation(current_degrees, glm::vec3(1.0, 1.0, 0.0));
    }
}

