#include "DCraft/Graphics/Geometry/Quad.h"

#include <vector>

#include "DCraft/Components/RenderableComponent.h"
#include "DCraft/Structs/Entity.h"

namespace DCraft {
    const std::vector<float> Quad::vertices_ = {
        1.0, 1.0, 0.0, // top-right
        -1.0, 1.0, 0.0, // top-left
        1.0, -1.0, 0.0, // bottom-left
        -1.0, -1.0, 0.0 // bottom-right
    };

    // Elements
    const std::vector<unsigned int> Quad::indices_ = {
        0, 1, 2, // First triangle
        2, 3, 1, // Second triangle
    };

    // uvs
    const std::vector<float> Quad::uvs_ = {
        1.0, 1.0,  // top-right
        0.0, 1.0,  // top-left
        1.0, 0.0,  // bottom-right
        0.0, 0.0   // bottom-left
    };
    
    Entity* Quad::create(const std::string& name, const glm::vec3& color) {
        // Create the entity
        auto* entity = new Entity(name);
        
        // Add renderable component
        auto* renderable = entity->add_component<RenderableComponent>();
        
        // Generate quad mesh data
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        get_quad_data(vertices, indices, color);
        
        // Create and set mesh
        auto mesh = std::make_shared<Mesh>(vertices, indices);
        renderable->set_mesh(mesh);
        
        // Update world matrices
        entity->update_world_matrices();
        
        return entity;
    }
    
    void Quad::get_quad_data(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, 
                            const glm::vec3& color) {
        vertices.clear();
        indices.clear();
        
        vertices.reserve(4);
        indices.assign(indices_.begin(), indices_.end());
        
        // Create vertex data
        for (size_t i = 0; i < 4; ++i) {
            Vertex v;
            v.position = glm::vec3(vertices_[i * 3], vertices_[i * 3 + 1], vertices_[i * 3 + 2]);
            v.color = color;
            
            // A quad faces forward (+Z direction)
            v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
            
            v.texCoords = glm::vec2(uvs_[i * 2], uvs_[i * 2 + 1]);
            vertices.push_back(v);
        }

    }
}
