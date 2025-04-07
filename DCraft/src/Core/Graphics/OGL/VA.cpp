#include "DCraft/Graphics/OGL/VA.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/detail/type_int.hpp>

VA::VA()
{
	glGenVertexArrays(1, &m_renderer_id_);
}

VA::~VA()
{
	glDeleteVertexArrays(1, &m_renderer_id_);
}

void VA::bind()
{
	glBindVertexArray(m_renderer_id_);
}

void VA::unbind()
{
	glBindVertexArray(0);
}

uint32_t VA::get_id()
{
	return m_renderer_id_;
}

