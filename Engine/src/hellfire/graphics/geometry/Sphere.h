//
// Created by denzel on 11/08/2025.
//
#pragma once
#include <string>
#include <vector>
#include <glm/detail/type_vec.hpp>
#include <glm/detail/type_vec3.hpp>

struct Vertex;

namespace hellfire {
    class Entity;

    class Sphere {
    public:
        // Factory method to create a sphere entity
        static Entity* create(const std::string& name = "Sphere", const glm::vec3& color = glm::vec3(1.0f), int rings = 32, int sectors = 32);

        static void get_sphere_data(std::vector<Vertex>& vertices, 
                           std::vector<unsigned int>& indices,
                           const glm::vec3& color = glm::vec3(1.0f),
                           int rings = 32, int sectors = 32);

    private:
        // Helper method for icosphere generation
        static void subdivide_triangle(std::vector<glm::vec3>& vertices,
                                      glm::vec3 v1, glm::vec3 v2, glm::vec3 v3,
                                      int depth);
    };
}
