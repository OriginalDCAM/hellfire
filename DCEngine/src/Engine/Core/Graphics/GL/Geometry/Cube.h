#pragma once
#include "../../../Object3D.h"

class Cube : public Object3D
{
	const std::vector<float> vertices = {
		// front
		-1.0, -1.0, 1.0,
		1.0, -1.0, 1.0,
		1.0, 1.0, 1.0,
		-1.0, 1.0, 1.0,
		// back
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0, 1.0, -1.0,
		-1.0, 1.0, -1.0,
	};

	// Colors
	const std::vector<float> colors = {
		// front colors
		1.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 1.0, 1.0,
		// back colors
		0.0, 1.0, 1.0,
		1.0, 0.0, 1.0,
		1.0, 0.0, 0.0,
		1.0, 1.0, 0.0,
	};

	// Elements
	const std::vector<unsigned int> cube_elements = {
		// Front face
		0, 1, 2,
		2, 3, 0,
		// Right face
		1, 5, 6,
		6, 2, 1,
		// Back face
		5, 4, 7,
		7, 6, 5,
		// Left face
		4, 0, 3,
		3, 7, 4,
		// Top face
		3, 2, 6,
		6, 7, 3,
		// Bottom face
		4, 5, 1,
		1, 0, 4
	};

public:
	Cube();

	void draw_self(const glm::mat4& view, const glm::mat4& projection, GLuint shader_program) override;

	void update() override;


};
