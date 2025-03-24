#pragma once
#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec3.hpp>

struct Vertex 
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texCoords;
};
