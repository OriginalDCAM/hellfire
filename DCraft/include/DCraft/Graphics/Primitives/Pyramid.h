#pragma once
#include "Shape.h"
#include "DCraft/Structs/Object3D.h"

namespace DCraft
{
    class Pyramid : public Shape
    {
        const std::vector<float> vertices = {
            // Plane vertices
            1.0, 0.0, 1.0,
            -1.0, 0.0, 1.0,
            -1.0, 0.0, -1.0,
            1.0, 0.0, 1.0,

            // Cone vertices

        };

        // Colors
        const std::vector<float> colors = {
            // front colors
            1.0, 1.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 1.0,
            1.0, 1.0, 1.0,
        };

        // Elements
        const std::vector<unsigned int> pyramid_elements = {
            0, 1, 2,
            2, 3, 1,
        };

    public:
        Pyramid();

        void draw_self(const glm::mat4& view, const glm::mat4& projection, GLuint shader_program) override;

        void update(float dt) override;
        void set_vertices() override;
        void set_colors() override;
        void set_uvs() override;
    };
}

