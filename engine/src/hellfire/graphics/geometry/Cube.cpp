#include "hellfire/graphics/geometry/Cube.h"
#include <iostream>
#include <glm/glm.hpp>

#include "hellfire/ecs/RenderableComponent.h"
#include "hellfire/ecs/TransformComponent.h"
#include "hellfire/ecs/components/MeshComponent.h"
#include "hellfire/graphics/Mesh.h"
#include "hellfire/scene/Scene.h"

namespace hellfire {
// Static geometry data
    const std::vector<float> Cube::vertices_ = {
        // Front face (Z+)
        -1.0f, -1.0f,  1.0f,   // 0: bottom-left
         1.0f, -1.0f,  1.0f,   // 1: bottom-right  
         1.0f,  1.0f,  1.0f,   // 2: top-right
        -1.0f,  1.0f,  1.0f,   // 3: top-left
        
        // Back face (Z-)
        -1.0f, -1.0f, -1.0f,   // 4: bottom-left
         1.0f, -1.0f, -1.0f,   // 5: bottom-right
         1.0f,  1.0f, -1.0f,   // 6: top-right  
        -1.0f,  1.0f, -1.0f,   // 7: top-left
        
        // Left face (X-)
        -1.0f, -1.0f, -1.0f,   // 8: bottom-back
        -1.0f, -1.0f,  1.0f,   // 9: bottom-front
        -1.0f,  1.0f,  1.0f,   // 10: top-front
        -1.0f,  1.0f, -1.0f,   // 11: top-back
        
        // Right face (X+)
         1.0f, -1.0f, -1.0f,   // 12: bottom-back
         1.0f, -1.0f,  1.0f,   // 13: bottom-front
         1.0f,  1.0f,  1.0f,   // 14: top-front
         1.0f,  1.0f, -1.0f,   // 15: top-back
        
        // Bottom face (Y-)
        -1.0f, -1.0f, -1.0f,   // 16: back-left
         1.0f, -1.0f, -1.0f,   // 17: back-right
         1.0f, -1.0f,  1.0f,   // 18: front-right
        -1.0f, -1.0f,  1.0f,   // 19: front-left
        
        // Top face (Y+)
        -1.0f,  1.0f, -1.0f,   // 20: back-left
         1.0f,  1.0f, -1.0f,   // 21: back-right
         1.0f,  1.0f,  1.0f,   // 22: front-right
        -1.0f,  1.0f,  1.0f    // 23: front-left
    };

    const std::vector<float> Cube::uvs_ = {
        // Front face
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
        // Back face
        1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
        // Left face
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
        // Right face
        1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
        // Bottom face
        0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f,
        // Top face
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f
    };

    const std::vector<unsigned int> Cube::indices_ = {
        // Front face
        0,  1,  2,    0,  2,  3,
        // Back face  
        5,  4,  7,    5,  7,  6,
        // Left face
        8,  9,  10,   8, 10, 11,
        // Right face
        13, 12, 15,   13, 15, 14,
        // Bottom face
        16, 17, 18,   16, 18, 19,
        // Top face
        23, 22, 21,   23, 21, 20
    };

    EntityID Cube::create(Scene* scene, const std::string& name, const Config& config) {
        EntityID id = scene->create_entity(name);
        Entity* entity = scene->get_entity(id);
        
        // Add mesh component
        auto* mesh_comp = entity->add_component<MeshComponent>();
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        get_cube_data(vertices, indices, config.color);
        mesh_comp->set_mesh(std::make_shared<Mesh>(vertices, indices));
        mesh_comp->set_source(MeshSource::INTERNAL);
        
        // Add renderable component
        auto* renderable = entity->add_component<RenderableComponent>();
        if (config.material) {
            renderable->set_material(config.material);
        } else {
            auto material = MaterialBuilder::create("Cube Material");
            renderable->set_material(material);
        }
        
        // Apply transform
        entity->transform()->set_position(config.position);
        entity->transform()->set_rotation(config.rotation);
        entity->transform()->set_scale(config.scale);
        
        return id;
    }

    std::shared_ptr<Mesh> Cube::create_mesh() {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        get_cube_data(vertices, indices);
        return std::make_shared<Mesh>(vertices, indices);
    }
    
    void Cube::get_cube_data(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, 
                            const glm::vec3& color) {
        vertices.clear();
        indices.clear();
        
        vertices.reserve(24); // 6 faces * 4 vertices each
        indices.assign(indices_.begin(), indices_.end());
        
        // Create vertex data
        for (size_t i = 0; i < 24; ++i) {
            Vertex v;
            v.position = glm::vec3(vertices_[i * 3], vertices_[i * 3 + 1], vertices_[i * 3 + 2]);
            v.color = color;
            
            // Determine face and assign normal
            int face = i / 4;
            switch (face) {
                case 0: v.normal = glm::vec3(0.0f, 0.0f, 1.0f);  break; // Front face (+Z)
                case 1: v.normal = glm::vec3(0.0f, 0.0f, -1.0f); break; // Back face (-Z)
                case 2: v.normal = glm::vec3(-1.0f, 0.0f, 0.0f); break; // Left face (-X)
                case 3: v.normal = glm::vec3(1.0f, 0.0f, 0.0f);  break; // Right face (+X)
                case 4: v.normal = glm::vec3(0.0f, -1.0f, 0.0f); break; // Bottom face (-Y)
                case 5: v.normal = glm::vec3(0.0f, 1.0f, 0.0f);  break; // Top face (+Y)
                default: v.normal = glm::vec3(0.0f, 1.0f, 0.0f); break;
            }
            
            v.texCoords = glm::vec2(uvs_[i * 2], uvs_[i * 2 + 1]);
            vertices.push_back(v);
        }
    }
}
