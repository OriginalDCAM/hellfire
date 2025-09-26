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

        ~Mesh();

        void cleanup();

        void bind() const;

        void unbind() const;

        // mesh data
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        std::shared_ptr<Material> material = nullptr;

        void set_material(const std::shared_ptr<Material> &mat) { material = mat; }
        [[nodiscard]] std::shared_ptr<Material> get_material() const { return material ? material : nullptr; }

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
