#pragma once
#include "Vertex.h"
#include "OGL/IB.h"
#include "OGL/VA.h"
#include "OGL/VB.h"
#include "Materials/Material.h"

namespace DCraft {
    struct Mesh {
    public:
        Mesh();
        Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);

        ~Mesh();

        void cleanup();

        void bind();

        void unbind();

        // mesh data
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        Material *material = nullptr;

        void set_material(Material *mat) { material = mat; }
        [[nodiscard]] Material *get_material() const { return material ? material : nullptr; }

        bool is_wireframe = false;

        void draw() const;

        void draw_instanced(size_t amount);

        int get_index_count();

    private:
        VA *vao_;
        VB *vbo_;
        IB *ibo_;

        int index_count_;

        void create_mesh();
    };
}
