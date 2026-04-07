#include "Dcraft/Graphics/Primitives/Cube.h"

#include <iostream>
#include <glm/gtc/type_ptr.inl>

namespace DCraft {
    Cube::Cube(const std::string& name) : Shape3D(name) {
        std::vector<Vertex> vertices_data;
        std::vector<unsigned int> indices_data;
        std::vector<Texture *> texture_data = {};

        // Create the cube's vertex data
        for (size_t i = 0; i < 24; ++i) {
            Vertex v;
            v.position = glm::vec3(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
            v.color = glm::vec3(color_);

            // Determine which face this vertex belongs to
            int face = i / 4;

            // Assign the normal based on the face
            switch (face) {
                case 0: v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
                    break; // Front face (+Z)
                case 1: v.normal = glm::vec3(0.0f, 0.0f, -1.0f);
                    break; // Back face (-Z)
                case 2: v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
                    break; // Top face (+Y)
                case 3: v.normal = glm::vec3(0.0f, -1.0f, 0.0f);
                    break; // Bottom face (-Y)
                case 4: v.normal = glm::vec3(1.0f, 0.0f, 0.0f);
                    break; // Right face (+X)
                case 5: v.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
                    break; // Left face (-X)
            }

            v.texCoords = glm::vec2(uvs[i * 2], uvs[i * 2 + 1]);
            vertices_data.push_back(v);
        }

        // Make sure to have enough memory to avoid having reallocations of memory 
        indices_data.reserve(std::size(cube_elements));

        // Convert element array to vector
        for (size_t i = 0; i < std::size(cube_elements); ++i) {
            indices_data.push_back(static_cast<unsigned int>(cube_elements[i]));
        }

        if (has_texture()) {
            texture_data.push_back(get_texture());
        }

        // Create mesh and add to the model
        Mesh* cubeMesh = new Mesh(vertices_data, indices_data);
        set_mesh(cubeMesh);

        // Initialize transformations
        update_world_matrix();
    }

    json Cube::to_json() {
        json j = Shape3D::to_json();
        j["geometry"] = "cube";
        return j;
    }

    void Cube::update(float dt) {

    }
}
