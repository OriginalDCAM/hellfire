#include "VB.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

VB::VB()
{
	glGenBuffers(1, &m_renderer_id_);
}

VB::~VB()
{
	glDeleteBuffers(1, &m_renderer_id_);
}

void VB::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id_);
}

void VB::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}
