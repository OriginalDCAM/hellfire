#include "hellfire/graphics/geometry/Quad.h"

#include <vector>

#include "hellfire/ecs/RenderableComponent.h"
#include "hellfire/ecs/Entity.h"
#include "hellfire/ecs/components/MeshComponent.h"
#include "hellfire/scene/Scene.h"

namespace hellfire {
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
    
    EntityID Quad::create(Scene* scene, const std::string& name, const Config& config) {
        EntityID id = scene->create_entity(name);
        Entity* entity = scene->get_entity(id);
        
        // Add mesh component
        auto* mesh_comp = entity->add_component<MeshComponent>();
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        get_quad_data(vertices, indices, config.color);
        mesh_comp->set_mesh(std::make_shared<Mesh>(vertices, indices));
        
        // Add renderable component
        auto* renderable = entity->add_component<RenderableComponent>();
        if (config.material) {
            renderable->set_material(config.material);
        } else {
            auto material = MaterialBuilder::create_lambert("Quad Material");
            renderable->set_material(material);
        }
        
        // Apply transform
        entity->transform()->set_position(config.position);
        entity->transform()->set_rotation(config.rotation);
        entity->transform()->set_scale(config.scale);
        
        return id;
    }

    std::shared_ptr<Mesh> Quad::create_mesh() {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        get_quad_data(vertices, indices);
        return std::make_shared<Mesh>(vertices, indices);
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
