#include "hellfire/graphics/Mesh.h"
#include <unordered_map>
#include "hellfire/core/Application.h"
#include "hellfire/graphics/Vertex.h"
#include "hellfire/graphics/material/Material.h"

namespace hellfire {
    Mesh::Mesh() : vao_(nullptr), vbo_(nullptr), ibo_(nullptr), index_count_(0) {
    }

    Mesh::Mesh(const std::vector<Vertex> &vertices,
               const std::vector<unsigned int> &indices) : vertices(vertices), indices(indices), index_count_(0) {
        create_mesh();
    }

    void Mesh::bind() const {
        vao_->bind();
    }

    void Mesh::unbind() const {
        vao_->unbind();
    }

    void Mesh::build() {
        create_mesh();
    }

    void Mesh::create_mesh() {
        vao_ = std::make_unique<VA>();
        vbo_ = std::make_unique<VB>();
        ibo_ = std::make_unique<IB>();

        vao_->bind();

        vbo_->bind();
        vbo_->pass_data(vertices);

        ibo_->bind();
        ibo_->pass_data(indices);

        // Layout 0: Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<void *>(offsetof(Vertex, position)));

        // Layout 1: Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<void *>(offsetof(Vertex, normal)));

        // Layout 2: Color 
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<void *>(offsetof(Vertex, color)));

        // Layout 3: texCoords (Uv's) 
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<void *>(offsetof(Vertex, texCoords)));

        // Layout 4: Tangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<void *>(offsetof(Vertex, tangent)));

        // Layout 5: Bi Tangent
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<void *>(offsetof(Vertex, bitangent)));

        // Unbind the buffers
        vao_->unbind();
        vbo_->unbind();
        ibo_->unbind();
    }

    void Mesh::draw() const {
        vao_->bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
        vao_->unbind();
    }

    void Mesh::draw_instanced(const size_t amount) const {
        vao_->bind();
        glDrawElementsInstanced(GL_TRIANGLES, get_index_count(),
                                GL_UNSIGNED_INT, nullptr, amount);
        vao_->unbind();
    }

    int Mesh::get_index_count() const {
        return indices.size();
    }
}
