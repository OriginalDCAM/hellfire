//
// Created by denzel on 11/08/2025.
//
#pragma once
#include <memory>
#include <string>
#include <vector>
#include <glm/detail/type_vec.hpp>
#include <glm/detail/type_vec3.hpp>

#include "hellfire/graphics/material/Material.h"

namespace hellfire {
    class Mesh;
    class Scene;
}

struct Vertex;

namespace hellfire {
    class Entity;
    using EntityID = uint32_t;

    class Sphere {
    public:
        struct Config {
            glm::vec3 color = glm::vec3(1.0f);
            std::shared_ptr<Material> material = nullptr;
            glm::vec3 position = glm::vec3(0.0f);
            glm::vec3 scale = glm::vec3(1.0f);
            int rings = 32;
            int sectors = 32;
        };

        static EntityID create(Scene *scene, const std::string &name, const Config &config);

        static std::shared_ptr<Mesh> create_mesh(int rings = 32, int sectors = 32);

        static void get_sphere_data(std::vector<Vertex> &vertices,
                                    std::vector<unsigned int> &indices,
                                    float radius, int rings = 32, int sectors = 32);

    private:
        // Helper method for icosphere generation
        static void subdivide_triangle(std::vector<glm::vec3> &vertices,
                                       glm::vec3 v1, glm::vec3 v2, glm::vec3 v3,
                                       int depth);
    };
}
