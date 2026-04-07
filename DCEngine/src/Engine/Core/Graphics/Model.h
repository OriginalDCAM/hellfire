#pragma once
#include <vector>
#include <glm/detail/type_mat4x4.hpp>

#include "Mesh.h"

struct Model 
{
	std::vector<Mesh> meshes;
	glm::mat4 transform = glm::mat4();

	void add_mesh(Mesh mesh)
	{
		meshes.push_back(mesh);
	}

};
