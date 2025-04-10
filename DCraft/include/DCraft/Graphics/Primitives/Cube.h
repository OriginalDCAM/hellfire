#pragma once
#include "Shape3D.h"
#include "DCraft/Structs/Object3D.h"

namespace DCraft
{

    class Cube : public Shape3D
    {
        const std::vector<float> vertices = {
            // Front face
            -1.0f, -1.0f,  1.0f,  // 0
             1.0f, -1.0f,  1.0f,  // 1
             1.0f,  1.0f,  1.0f,  // 2
            -1.0f,  1.0f,  1.0f,  // 3
            
            // Back face
            -1.0f, -1.0f, -1.0f,  // 4
             1.0f, -1.0f, -1.0f,  // 5
             1.0f,  1.0f, -1.0f,  // 6
            -1.0f,  1.0f, -1.0f,  // 7
            
            // Top face
            -1.0f,  1.0f, -1.0f,  // 8
             1.0f,  1.0f, -1.0f,  // 9
             1.0f,  1.0f,  1.0f,  // 10
            -1.0f,  1.0f,  1.0f,  // 11
            
            // Bottom face
            -1.0f, -1.0f, -1.0f,  // 12
             1.0f, -1.0f, -1.0f,  // 13
             1.0f, -1.0f,  1.0f,  // 14
            -1.0f, -1.0f,  1.0f,  // 15
            
            // Right face
             1.0f, -1.0f, -1.0f,  // 16
             1.0f,  1.0f, -1.0f,  // 17
             1.0f,  1.0f,  1.0f,  // 18
             1.0f, -1.0f,  1.0f,  // 19
            
            // Left face
            -1.0f, -1.0f, -1.0f,  // 20
            -1.0f,  1.0f, -1.0f,  // 21
            -1.0f,  1.0f,  1.0f,  // 22
            -1.0f, -1.0f,  1.0f   // 23
        };

        // Colors for each vertex
        const std::vector<float> colors = {
            // // Front face (red)
            // 1.0f, 0.0f, 0.0f,
            // 1.0f, 0.0f, 0.0f,
            // 1.0f, 0.0f, 0.0f,
            // 1.0f, 0.0f, 0.0f,
            //
            // // Back face (green)
            // 0.0f, 1.0f, 0.0f,
            // 0.0f, 1.0f, 0.0f,
            // 0.0f, 1.0f, 0.0f,
            // 0.0f, 1.0f, 0.0f,
            //
            // // Top face (blue)
            // 0.0f, 0.0f, 1.0f,
            // 0.0f, 0.0f, 1.0f,
            // 0.0f, 0.0f, 1.0f,
            // 0.0f, 0.0f, 1.0f,
            //
            // // Bottom face (yellow)
            // 1.0f, 1.0f, 0.0f,
            // 1.0f, 1.0f, 0.0f,
            // 1.0f, 1.0f, 0.0f,
            // 1.0f, 1.0f, 0.0f,
            //
            // // Right face (magenta)
            // 1.0f, 0.0f, 1.0f,
            // 1.0f, 0.0f, 1.0f,
            // 1.0f, 0.0f, 1.0f,
            // 1.0f, 0.0f, 1.0f,
            //
            // // Left face (cyan)
            // 0.0f, 1.0f, 1.0f,
            // 0.0f, 1.0f, 1.0f,
            // 0.0f, 1.0f, 1.0f,
            // 0.0f, 1.0f, 1.0f
        };

        // UV coordinates for each vertex
        const std::vector<float> uvs = {
            // Front face
            0.0f, 0.0f,  // bottom-left
            1.0f, 0.0f,  // bottom-right
            1.0f, 1.0f,  // top-right
            0.0f, 1.0f,  // top-left
            
            // Back face
            0.0f, 0.0f,  // bottom-left
            1.0f, 0.0f,  // bottom-right
            1.0f, 1.0f,  // top-right
            0.0f, 1.0f,  // top-left
            
            // Top face
            0.0f, 0.0f,  // bottom-left
            1.0f, 0.0f,  // bottom-right
            1.0f, 1.0f,  // top-right
            0.0f, 1.0f,  // top-left
            
            // Bottom face
            0.0f, 0.0f,  // bottom-left
            1.0f, 0.0f,  // bottom-right
            1.0f, 1.0f,  // top-right
            0.0f, 1.0f,  // top-left
            
            // Right face
            0.0f, 0.0f,  // bottom-left
            1.0f, 0.0f,  // bottom-right
            1.0f, 1.0f,  // top-right
            0.0f, 1.0f,  // top-left
            
            // Left face
            0.0f, 0.0f,  // bottom-left
            1.0f, 0.0f,  // bottom-right
            1.0f, 1.0f,  // top-right
            0.0f, 1.0f   // top-left
        };

        // Indices for all 6 faces (36 indices total)
        const std::vector<unsigned int> cube_elements = {
            // Front face
            0, 1, 2,
            2, 3, 0,
            
            // Back face
            4, 5, 6,
            6, 7, 4,
            
            // Top face
            8, 9, 10,
            10, 11, 8,
            
            // Bottom face
            12, 13, 14,
            14, 15, 12,
            
            // Right face
            16, 17, 18,
            18, 19, 16,
            
            // Left face
            20, 21, 22,
            22, 23, 20
        };
        

    public:
        Cube();
        Cube(const std::string& name);

        void update(float dt) override;
    };
}
