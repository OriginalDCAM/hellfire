#include "VA.h"

#include <GL/glew.h>
#include <glm/detail/type_int.hpp>

VA::VA()
{
	glGenVertexArrays(1, &m_renderer_id_);
}

VA::~VA()
{
	glDeleteVertexArrays(1, &m_renderer_id_);
}

void VA::bind() const {
	glBindVertexArray(m_renderer_id_);
}

void VA::unbind()
{
	glBindVertexArray(0);
}

uint32_t VA::get_id() const {
	return m_renderer_id_;
}

