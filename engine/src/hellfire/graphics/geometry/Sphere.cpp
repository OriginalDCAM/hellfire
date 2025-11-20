//
// Created by denzel on 11/08/2025.
//
#include <hellfire/graphics/geometry/Sphere.h>

#include "hellfire/ecs/RenderableComponent.h"
#include "hellfire/ecs/TransformComponent.h"
#include "hellfire/ecs/components/MeshComponent.h"
#include "hellfire/scene/Scene.h"

namespace hellfire {
    EntityID Sphere::create(Scene *scene, const std::string &name, const Config &config) {
        const EntityID id = scene->create_entity(name);
        Entity *entity = scene->get_entity(id);

        // Add mesh component
        auto *mesh_comp = entity->add_component<MeshComponent>();
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Generate sphere geometry
        get_sphere_data(vertices, indices, 1.0f, config.rings, config.sectors);

        // Apply color to vertices
        for (auto &vertex: vertices) {
            vertex.color = config.color;
        }

        mesh_comp->set_mesh(std::make_shared<Mesh>(vertices, indices));
        mesh_comp->set_source(MeshSource::INTERNAL);

        // Add renderable component
        auto *renderable = entity->add_component<RenderableComponent>();
        if (config.material) {
            renderable->set_material(config.material);
        } else {
            const auto material = MaterialBuilder::create("Sphere Material");
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

    void Sphere::get_sphere_data(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices,
                                 float radius, int rings, int sectors) {
        vertices.clear();
        indices.clear();

        // Generate vertices
        for (int ring = 0; ring <= rings; ++ring) {
            constexpr float PI = 3.14159265359f;
            const float phi = PI * static_cast<float>(ring) / static_cast<float>(rings);
            float y = cos(phi);
            const float ring_radius = sin(phi);

            for (int sector = 0; sector <= sectors; ++sector) {
                const float theta = 2.0f * PI * static_cast<float>(sector) / static_cast<float>(sectors);
                float x = ring_radius * cos(theta);
                float z = ring_radius * sin(theta);

                Vertex vertex;
                vertex.normal = glm::normalize(glm::vec3(x, y, z));
                vertex.position = vertex.normal * radius;
                vertex.color = glm::vec3(1.0f);

                // UV coordinates
                vertex.texCoords = glm::vec2(
                    static_cast<float>(sector) / static_cast<float>(sectors),
                    static_cast<float>(ring) / static_cast<float>(rings)
                );

                vertices.push_back(vertex);
            }
        }

        for (int ring = 0; ring < rings; ++ring) {
            for (int sector = 0; sector < sectors; ++sector) {
                const int current = ring * (sectors + 1) + sector;
                const int next = current + sectors + 1;

                // Create two triangles for each quad
                indices.push_back(current);
                indices.push_back(current + 1);
                indices.push_back(next);

                indices.push_back(current + 1);
                indices.push_back(next + 1);
                indices.push_back(next);
            }
        }
    }

    void Sphere::subdivide_triangle(std::vector<glm::vec3> &vertices, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3,
                                    const int depth) {
        if (depth == 0) {
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            return;
        }

        // Calculate midpoints and normalize to sphere surface
        const glm::vec3 v12 = glm::normalize((v1 + v2) * 0.5f);
        const glm::vec3 v23 = glm::normalize((v2 + v3) * 0.5f);
        const glm::vec3 v31 = glm::normalize((v3 + v1) * 0.5f);

        // Recursively subdivide
        subdivide_triangle(vertices, v1, v12, v31, depth - 1);
        subdivide_triangle(vertices, v2, v23, v12, depth - 1);
        subdivide_triangle(vertices, v3, v31, v23, depth - 1);
        subdivide_triangle(vertices, v12, v23, v31, depth - 1);
    }
};
