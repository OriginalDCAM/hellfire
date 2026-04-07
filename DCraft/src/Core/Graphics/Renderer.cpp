#include "DCraft/Graphics/Renderer.h"
#include "GL/glew.h"

#include <glm/gtc/type_ptr.inl>

namespace DCraft
{
    Renderer::Renderer(uint32_t program_id_) : program_id_(program_id_)
    {
        uniform_mvp_ = glGetUniformLocation(program_id_, "MVP");
    }

    void Renderer::render(Object3D& scene, Camera& camera)
    {
        // Activate Shader
        glUseProgram(program_id_);
        
        glm::mat4 view = camera.get_view_matrix();
        glm::mat4 projection = camera.get_projection_matrix();

        scene.draw(view, projection, program_id_);
    }

    void Renderer::begin_frame()
    {
        // Setup frame
        glClearColor(0.1f, 0.2f, 0.3f, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::end_frame()
    {

    }
}



