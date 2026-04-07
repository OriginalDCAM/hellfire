#pragma once
#include "Vertex.h"
#include "backends/opengl/IB.h"
#include "backends/opengl/VA.h"
#include "backends/opengl/VB.h"
#include "material/Material.h"

namespace hellfire {
    class Mesh {
    public:
        Mesh();

        Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);

        Mesh(const std::vector<Vertex> &vertices,
             const std::vector<unsigned int> &indices,
             bool defer_build);

        void cleanup();

        void bind() const;

        void unbind() const;

        void build();
        bool is_built() const { return  vao_ != nullptr; }

        // mesh data
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        bool is_wireframe = false;

        void draw() const;

        void draw_instanced(size_t amount) const;

        int get_index_count() const;

    private:
        std::unique_ptr<VA> vao_ = nullptr;
        std::unique_ptr<VB> vbo_ = nullptr;
        std::unique_ptr<IB> ibo_ = nullptr;

        int index_count_;

        void create_mesh();
    };
}
