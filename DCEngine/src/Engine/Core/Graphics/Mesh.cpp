#include "Mesh.h"

#include <GL/glut.h>

#include "Vertex.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures) : vertices(vertices), indices(indices), textures(textures)
{
	create_mesh();
}

Mesh::~Mesh()
{
}

void Mesh::create_mesh()
{
	VAO = new VA();
	VBO = new VB();
	IBO = new IB();

	VAO->bind();

	VBO->bind();
	VBO->pass_data(vertices);

	IBO->bind();
	IBO->pass_data(indices);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	// Unbind the buffers
	VAO->unbind();
	VBO->unbind();
	IBO->unbind();
}

void Mesh::draw_mesh() const
{
	VAO->bind();

	if (is_wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	if (is_wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	VAO->unbind();
}

