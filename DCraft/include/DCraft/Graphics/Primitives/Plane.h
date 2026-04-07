#pragma once
#include "Shape.h"

namespace DCraft
{

    class Plane : public Shape
    {
        const std::vector<float> vertices = {
            1.0, 1.0, 0.0,
            -1.0, 1.0, 0.0,
            1.0, -1.0, 0.0,
            -1.0, -1.0, 0.0
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
        const std::vector<unsigned int> plane_elements = {
            0, 1, 2,
            2, 3, 1,
        };

        // uvs
        const std::vector<float> uvs = {
            1.0, 1.0,  // top-right
            0.0, 1.0,  // top-left
            1.0, 0.0,  // bottom-right
            0.0, 0.0   // bottom-left
        };

    public:
        Plane();

        void draw_self(const glm::mat4& view, const glm::mat4& projection, GLuint shader_program) override;

        void update(float dt) override;
        void set_vertices() override;
        void set_colors() override;
        void set_uvs() override;
    };
}
