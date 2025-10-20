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
        0, 1, 3, // First triangle
        0, 3, 2, // Second triangle
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
        mesh_comp->set_source(MeshSource::INTERNAL);
        
        // Add renderable component
        auto* renderable = entity->add_component<RenderableComponent>();
        if (config.material) {
            renderable->set_material(config.material);
        } else {
            auto material = MaterialBuilder::create("Quad Material");
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

        // Extract the four corner positions
        glm::vec3 p0(vertices_[0], vertices_[1], vertices_[2]);
        glm::vec3 p1(vertices_[3], vertices_[4], vertices_[5]);
        glm::vec3 p2(vertices_[6], vertices_[7], vertices_[8]);
    
        // Calculate two edge vectors
        glm::vec3 edge1 = p1 - p0;
        glm::vec3 edge2 = p2 - p0;

        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        // Create vertex data
        for (size_t i = 0; i < 4; ++i) {
            Vertex v;
            v.position = glm::vec3(vertices_[i * 3], vertices_[i * 3 + 1], vertices_[i * 3 + 2]);
            v.color = color;
            v.normal = normal;
            v.texCoords = glm::vec2(uvs_[i * 2], uvs_[i * 2 + 1]);
            vertices.push_back(v);
        }
    }
}
