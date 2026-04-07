//
// Created by denzel on 14/06/2025.
//

#include "hellfire/graphics/renderer/SkyboxRenderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "../core/Application.h"
#include <GL/glew.h>

#include "Mesh.h"
#include "Skybox.h"
#include "Geometry/Cube.h"
#include "hellfire/utilities/ServiceLocator.h"

namespace hellfire {
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

    void SkyboxRenderer::initialize() {
        if (!initialized_) {
            setup_skybox_geometry();
            load_skybox_shader();
        }
    }

    void SkyboxRenderer::setup_skybox_geometry() {
        skybox_mesh_ = Cube::create_mesh();
    }

    void SkyboxRenderer::load_skybox_shader() {
        skybox_shader_id_ = ServiceLocator::get_service<ShaderManager>()->load_shader_from_files(
            "assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
        skybox_shader_ = Shader::from_id(skybox_shader_id_);
    }

    void SkyboxRenderer::render(const Skybox &skybox, const CameraComponent *camera) const {
        if (!skybox.is_loaded() || !skybox_shader_.is_valid()) return;

        // Save current depth state
        glDepthFunc(GL_LEQUAL);
        skybox_shader_.use();

        // Remove translation from view matrix
        glm::mat4 view = glm::mat4(glm::mat3(camera->get_view_matrix()));
        glm::mat4 projection = camera->get_projection_matrix();
        
        skybox_shader_.set_mat4("view", view);
        skybox_shader_.set_mat4("projection", projection);

        // Set tint and exposure
        skybox_shader_.set_vec3("tint", skybox.get_tint());
        skybox_shader_.set_float("exposure", skybox.get_exposure());

        // Bind cubemap
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.get_cubemap());
        skybox_shader_.set_int("skyboxes", 0);

        // Draw skyboxes cube
        skybox_mesh_->bind();
        skybox_mesh_->draw();
        skybox_mesh_->unbind();

        // Restore depth state
        glDepthFunc(GL_LESS);
    }
}
