#include "DCraft/Graphics/Mesh.h"

#include <iostream>
#include <unordered_map>
#include <GL/glut.h>

#include "DCraft/Graphics/Vertex.h"
#include "DCraft/Graphics/Materials/Material.h"

namespace DCraft {
    Mesh::Mesh() {
    }

    Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices) : vertices(vertices), indices(indices) {
        create_mesh();
    }

    Mesh::~Mesh() {
        // delete VAO;
        // delete VBO;
        // delete IBO;
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));

        // Layout 1: Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));

        // Layout 2: Color 
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, color));

        // Layout 2: texCoords (Uv's) 
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texCoords));

        // Unbind the buffers
        VAO->unbind();
        VBO->unbind();
        IBO->unbind();
    }

    // Object3D* Mesh::create_mesh(Geometry& geometry, Material& material) {
    // Implement this, Claude don't give me the implementation please would like to do this myself.
    // }

    void Mesh::draw() const {
        if (!material) return;
        uint32_t shader_program = material->get_shader();
        glUseProgram(shader_program);
        
        VAO->bind();

        const auto& textures = material->get_textures();

        glUniform1i(glGetUniformLocation(shader_program, "useTexture"), !textures.empty());

        uint32_t tex_unit = 0;
        std::unordered_map<TextureType, uint32_t> counters;

        for (Texture* tex : textures) {
            glActiveTexture(GL_TEXTURE0 + tex_unit);

            std::string uniform_name;
            switch (tex->get_type()) {
                case TextureType::DIFFUSE:
                    uniform_name = "diffuse" + std::to_string(++counters[TextureType::DIFFUSE]);
                    break;
                case TextureType::SPECULAR:
                    uniform_name = "specular" + std::to_string(++counters[TextureType::SPECULAR]);
                    break;
                case TextureType::NORMAL:
                    uniform_name = "normal" + std::to_string(++counters[TextureType::NORMAL]);
                    break;
                case TextureType::ROUGHNESS:
                    uniform_name = "rougness" + std::to_string(++counters[TextureType::ROUGHNESS]);
                    break;
                case TextureType::METALLNESS:
                    uniform_name = "metalness" + std::to_string(++counters[TextureType::METALLNESS]);
                    break;
                case TextureType::AMBIENT_OCCLUSION:
                    uniform_name = "ao" + std::to_string(++counters[TextureType::AMBIENT_OCCLUSION]);
                    break;
            }

            // Set sampler to correct texture unit
            glUniform1i(glGetUniformLocation(shader_program, uniform_name.c_str()), tex_unit);

            tex->bind();
            tex_unit++;
        }

        glPolygonMode(GL_FRONT_AND_BACK, is_wireframe ? GL_LINE : GL_FILL);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Always restore

        VAO->unbind();
    }
}