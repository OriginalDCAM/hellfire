#include "Renderer.h"

#include <GL/freeglut_std.h>
#include <glm/gtc/type_ptr.inl>


class VA;

Renderer::Renderer(uint32_t program_id_) : program_id_(program_id_)
{
	uniform_mvp_ = glGetUniformLocation(program_id_, "MVP");
}

void Renderer::Render(Object3D& scene, Camera& camera)
{
	// Setup frame
	glClearColor(0.5, 0.5, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate Shader
	glUseProgram(program_id_);

	glm::mat4 view = camera.get_view_matrix();
	glm::mat4 projection = camera.get_projection_matrix();


	scene.draw(view, projection, program_id_);

	glutSwapBuffers();
}


