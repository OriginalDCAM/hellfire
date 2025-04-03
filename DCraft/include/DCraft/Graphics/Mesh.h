#pragma once
#include "DCraft/Graphics/Textures/Texture.h"
#include "Vertex.h"
#include "GL/IB.h"
#include "GL/VA.h"
#include "GL/VB.h"

namespace DCraft
{

    struct Mesh
    {
    public:
        // mesh data
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture*> textures;

        bool is_wireframe = false;

        Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture*>& textures);
        ~Mesh();
        
        void update_textures(const std::vector<Texture*>& new_textures) {
            textures = new_textures;
        }

        void draw_mesh(uint32_t shader_program) const;
    private:
        VA* VAO;
        VB* VBO;
        IB* IBO;

        void create_mesh();
    };
}
