//
// Created by denzel on 11/08/2025.
//
#include <hellfire/graphics/geometry/Sphere.h>

#include "hellfire/ecs/RenderableComponent.h"
#include "hellfire/ecs/components/MeshComponent.h"
#include "hellfire/scene/Scene.h"

namespace hellfire {
    EntityID Sphere::create(Scene *scene, const std::string &name, const Config &config) {
        EntityID id = scene->create_entity(name);
        Entity* entity = scene->get_entity(id);

        // Add mesh component
        auto* mesh_comp = entity->add_component<MeshComponent>();
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Generate sphere geometry
        get_sphere_data(vertices, indices, 1.0f, config.rings, config.sectors);

        // Apply color to vertices
        for (auto& vertex : vertices) {
            vertex.color = config.color;
        }

        mesh_comp->set_mesh(std::make_shared<Mesh>(vertices, indices));

        // Add renderable component
        auto* renderable = entity->add_component<RenderableComponent>();
        if (config.material) {
            renderable->set_material(config.material);
        } else {
            auto material = MaterialBuilder::create_lambert("Sphere Material");
            renderable->set_material(material);
        }
        
        // Apply transform
        entity->transform()->set_position(config.position);
        entity->transform()->set_scale(config.scale);
        
        return id;
    }

    std::shared_ptr<Mesh> Sphere::create_mesh(int rings, int sectors) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        get_sphere_data(vertices, indices, 1.0f, rings, sectors);
        return std::make_shared<Mesh>(vertices, indices);
    }

    void Sphere::get_sphere_data(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                                 float radius, int rings, int sectors) {
        vertices.clear();
        indices.clear();

        const float PI = 3.14159265359f; // 16 bit precision

        // Generate vertices
        for (int ring = 0; ring <= rings; ++ring) {
            float phi = PI * ring / rings; 
            float y = cos(phi);
            float ring_radius = sin(phi);

            for (int sector = 0; sector <= sectors; ++sector) {
                float theta = 2.0f * PI * sector / sectors; 
                float x = ring_radius * cos(theta);
                float z = ring_radius * sin(theta);

                Vertex vertex;
                vertex.position = glm::vec3(x, y, z) * radius;
                vertex.normal = glm::normalize(vertex.position); 
                vertex.color = glm::vec3(1.0f);

                // UV coordinates
                vertex.texCoords = glm::vec2(
                    (float) sector / sectors,
                    (float) ring / rings
                );

                vertices.push_back(vertex);
            }
        }

        for (int ring = 0; ring < rings; ++ring) {
            for (int sector = 0; sector < sectors; ++sector) {
                int current = ring * (sectors + 1) + sector;
                int next = current + sectors + 1;

                // Create two triangles for each quad
                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(current + 1);

                indices.push_back(current + 1);
                indices.push_back(next);
                indices.push_back(next + 1);
            }
        }
    }

    void Sphere::subdivide_triangle(std::vector<glm::vec3> &vertices, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3,
                                    int depth) {
        if (depth == 0) {
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            return;
        }

        // Calculate midpoints and normalize to sphere surface
        glm::vec3 v12 = glm::normalize((v1 + v2) * 0.5f);
        glm::vec3 v23 = glm::normalize((v2 + v3) * 0.5f);
        glm::vec3 v31 = glm::normalize((v3 + v1) * 0.5f);

        // Recursively subdivide
        subdivide_triangle(vertices, v1, v12, v31, depth - 1);
        subdivide_triangle(vertices, v2, v23, v12, depth - 1);
        subdivide_triangle(vertices, v3, v31, v23, depth - 1);
        subdivide_triangle(vertices, v12, v23, v31, depth - 1);
    }
};
