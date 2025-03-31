#pragma once
#include <vector>
#include <glm/detail/type_mat4x4.hpp>

#include "Mesh.h"

namespace DCraft
{
    class Model
    {
    public:
        std::vector<Mesh> meshes;
        glm::mat4 transform = glm::mat4();

        void add_mesh(const Mesh& mesh)
        {
            meshes.push_back(mesh);
        }

    };
}

