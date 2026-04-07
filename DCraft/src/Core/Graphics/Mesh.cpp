#include "DCraft/Graphics/Mesh.h"

#include <iostream>
#include <unordered_map>
#include <GL/glut.h>

#include "DCraft/Application.h"
#include "DCraft/Graphics/Vertex.h"
#include "DCraft/Graphics/Materials/Material.h"

namespace DCraft {
    Mesh::Mesh() : index_count_(0) {
    }

    Mesh::Mesh(const std::vector<Vertex> &vertices,
               const std::vector<unsigned int> &indices) : vertices(vertices), indices(indices), index_count_(0) {
        create_mesh();
    }

    Mesh::~Mesh() {
        cleanup();
    }

    void Mesh::cleanup() {
        delete vao_;
        delete vbo_;
        delete ibo_;
        
        vao_ = nullptr;
        vbo_ = nullptr;
        ibo_ = nullptr;
    }

    void Mesh::bind() {
        vao_->bind();
    }

    void Mesh::unbind() {
        vao_->unbind();
    }

    void Mesh::create_mesh() {
        vao_ = new VA();
        vbo_ = new VB();
        ibo_ = new IB();

        vao_->bind();

        vbo_->bind();
        vbo_->pass_data(vertices);

        ibo_->bind();
        ibo_->pass_data(indices);

        // Layout 0: Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, position)));

        // Layout 1: Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, normal)));

        // Layout 2: Color 
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, color)));

        // Layout 2: texCoords (Uv's) 
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, texCoords)));

        // Unbind the buffers
        vao_->unbind();
        vbo_->unbind();
        ibo_->unbind();
    }

    void Mesh::draw() const {
        if (!material) return;
        
        vao_->bind();

        glPolygonMode(GL_FRONT_AND_BACK, is_wireframe ? GL_LINE : GL_FILL);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Always restore

        vao_->unbind();
    }

    void Mesh::draw_instanced(size_t amount) {
        if (!material) return;

        vao_->bind();

        glDrawElementsInstanced(GL_TRIANGLES, get_index_count(),
                                GL_UNSIGNED_INT, 0, amount);

        vao_->unbind();
    }

   int Mesh::get_index_count() {
        return indices.size();
    }
}
