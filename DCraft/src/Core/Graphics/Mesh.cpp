#include "DCraft/Graphics/Mesh.h"

#include <iostream>
#include <unordered_map>
#include <GL/glut.h>

#include "DCraft/Application.h"
#include "DCraft/Graphics/Vertex.h"
#include "DCraft/Graphics/Materials/Material.h"

namespace DCraft {
    Mesh::Mesh() {
    }

    Mesh::Mesh(const std::vector<Vertex> &vertices,
               const std::vector<unsigned int> &indices) : vertices(vertices), indices(indices) {
        create_mesh();
    }

    Mesh::~Mesh() {
        cleanup();
    }

    void Mesh::cleanup() {
        if (VAO) delete VAO;
        if (VBO) delete VBO;
        if (IBO) delete IBO;
        
        VAO = nullptr;
        VBO = nullptr;
        IBO = nullptr;
    }

    void Mesh::create_mesh() {
        VAO = new VA();
        VBO = new VB();
        IBO = new IB();

        VAO->bind();

        VBO->bind();
        VBO->pass_data(vertices);

        IBO->bind();
        IBO->pass_data(indices);

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
        VAO->unbind();
        VBO->unbind();
        IBO->unbind();
    }

    void Mesh::draw() const {
        if (!material) return;
        
        VAO->bind();

        glPolygonMode(GL_FRONT_AND_BACK, is_wireframe ? GL_LINE : GL_FILL);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Always restore

        VAO->unbind();
    }
}
