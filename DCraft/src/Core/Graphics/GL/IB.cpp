#include "DCraft/Graphics/GL/IB.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdint>

IB::IB()
{
	glGenBuffers(1, &m_renderer_id_);
}

IB::~IB()
{
	glDeleteBuffers(1, &m_renderer_id_);
}

void IB::bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderer_id_);
}

void IB::unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
