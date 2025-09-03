//
// Created by denzel on 14/06/2025.
//

#include "DCraft/Graphics/Renderers/SkyboxRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "DCraft/Application.h"
#include <GL/glew.h>
#include "DCraft/Structs/Skybox.h"

namespace DCraft {
    // skyboxes cube vertices
    float skyboxVertices[] = {
        // positions          
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };

    SkyboxRenderer::~SkyboxRenderer() {
        if (skybox_vao_ != 0)
            glDeleteVertexArrays(1, &skybox_vao_);
        if (skybox_vbo_ != 0)
            glDeleteBuffers(1, &skybox_vbo_);
        if (skybox_shader_ != 0)
            glDeleteProgram(skybox_shader_);
    }

    void SkyboxRenderer::initialize() {
        if (!initialized_) {
            setup_skybox_geometry();
            load_skybox_shader();
        }
    }

    void SkyboxRenderer::setup_skybox_geometry() {
        glGenVertexArrays(1, &skybox_vao_);
        glGenBuffers(1, &skybox_vbo_);

        glBindVertexArray(skybox_vao_);
        glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void *>(nullptr));

        glBindVertexArray(0);
    }

    void SkyboxRenderer::load_skybox_shader() {
        // TODO: Make this more scalable
        skybox_shader_ = Application::get_instance().get_shader_manager().load_shader_from_files(
            "assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
    }

    void SkyboxRenderer::render(const Skybox *skybox, const CameraComponent *camera) const {
        if (!skybox || !skybox->is_loaded()) return;

        // Save current depth state
        glDepthFunc(GL_LEQUAL);

        glUseProgram(skybox_shader_);

        // Remove translation from view matrix
        glm::mat4 view = glm::mat4(glm::mat3(camera->get_view_matrix()));
        glm::mat4 projection = camera->get_projection_matrix();

        glUniformMatrix4fv(glGetUniformLocation(skybox_shader_, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skybox_shader_, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Set tint and exposure
        glUniform3fv(glGetUniformLocation(skybox_shader_, "tint"), 1, glm::value_ptr(skybox->get_tint()));
        glUniform1f(glGetUniformLocation(skybox_shader_, "exposure"), skybox->get_exposure());

        // Bind cubemap
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->get_cubemap());
        glUniform1i(glGetUniformLocation(skybox_shader_, "skyboxes"), 0);

        // Draw skyboxes cube
        glBindVertexArray(skybox_vao_);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // Restore depth state
        glDepthFunc(GL_LESS);
    }
}
