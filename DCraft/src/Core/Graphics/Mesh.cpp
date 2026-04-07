#include "DCraft/Graphics/Mesh.h"

#include <iostream>
#include <GL/glut.h>

#include "DCraft/Graphics/Vertex.h"
#include "DCraft/Graphics/Materials/Material.h"

namespace DCraft
{
    Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture*>& textures) : vertices(vertices), indices(indices), textures(textures)
    {
        create_mesh();
    }

    Mesh::~Mesh()
    {
        // delete VAO;
        // delete VBO;
        // delete IBO;
    }

    void Mesh::create_mesh()
    {
        VAO = new VA();
        VBO = new VB();
        IBO = new IB();

        VAO->bind();

        VBO->bind();
        VBO->pass_data(vertices);

        IBO->bind();
        IBO->pass_data(indices);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

        // Unbind the buffers
        VAO->unbind();
        VBO->unbind();
        IBO->unbind();
    }

    // Object3D* Mesh::create_mesh(Geometry& geometry, Material& material) {
        // Implement this, Claude don't give me the implementation please would like to do this myself.
    // }

    void Mesh::draw_mesh(uint32_t shader_program) const {
        VAO->bind();

        uint32_t diffuseNr = 1;
        uint32_t specularNr = 1;

        glUniform1i(glGetUniformLocation(shader_program, "useTexture"), textures.size() > 0 ? 1 : 0);
        
        for (uint32_t i = 0; i < textures.size(); i++) {
            // std::cout << "Binding texture " << textures[i]->get_id() << " to unit " << i << std::endl;
            // Activate texture unit
            glActiveTexture(GL_TEXTURE0 + i);
            
            std::string uniform_name;
            switch (textures[i]->type) {
                case TextureType::DIFFUSE:
                    uniform_name = "diffuse" + std::to_string(diffuseNr++);
                    break;
                case TextureType::SPECULAR:
                    uniform_name = "specular" + std::to_string(specularNr++);
                    break;
                case TextureType::NORMAL:
                    // TODO: implement this
                    break;
            }
            
            // Set sampler to correct texture unit
            glUniform1i(glGetUniformLocation(shader_program, uniform_name.c_str()), i);

            // Bind the texture
            textures[i]->bind();
        }

        if (is_wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        if (is_wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        VAO->unbind();
    }
}
