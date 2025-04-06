#pragma once
#include "Vertex.h"
#include "GL/IB.h"
#include "GL/VA.h"
#include "GL/VB.h"
#include "Materials/Material.h"

namespace DCraft {
    struct Mesh {
    public:
        Mesh();
        Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices);

        ~Mesh();

        // mesh data
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        Material *material = nullptr;

        void set_material(Material *mat) { material = mat; }
        Material *get_material() const { return material; }

        bool is_wireframe = false;

        void draw() const;

    private:
        VA *VAO;
        VB *VBO;
        IB *IBO;

        void create_mesh();
    };
}
