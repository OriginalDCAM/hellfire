#include "DCraft/Graphics/Renderer.h"
#include "GL/glew.h"

#include <glm/gtc/type_ptr.inl>

namespace DCraft {
    class PointLight;

    Renderer::Renderer(uint32_t program_id_) : program_id_(program_id_) {
        uniform_mvp_ = glGetUniformLocation(program_id_, "MVP");

        OGLRendererContext *ogl_context = new OGLRendererContext();
        ogl_context->default_shader_program = program_id_;
        context_ = static_cast<void *>(ogl_context);
    }

    Renderer::~Renderer() {
        if (context_) {
            OGLRendererContext *ogl_context = static_cast<OGLRendererContext *>(context_);
            delete ogl_context;
            context_ = nullptr;
        }
    }


    void Renderer::collect_lights(Object3D *object_3d, std::vector<DirectionalLight *> &dir_lights,
                                  std::vector<PointLight *> &point_lights) {
        // Check if this object is a light
        if (auto *dir_light = dynamic_cast<DirectionalLight *>(object_3d)) {
            dir_lights.push_back(dir_light);
        } else if (auto *point_light = dynamic_cast<PointLight *>(object_3d)) {
            point_lights.push_back(point_light);
        }

        // Check all the children
        for (auto *child: object_3d->get_children()) {
            collect_lights(child, dir_lights, point_lights);
        }
    }

    void Renderer::upload_lights(uint32_t shader_program, const std::vector<DirectionalLight *> &dir_lights,
                                 const std::vector<PointLight *> &point_lights) {
        glUniform1i(glGetUniformLocation(shader_program, "numDirectionalLights"), static_cast<int>(dir_lights.size()));
        glUniform1i(glGetUniformLocation(shader_program, "numPointLights"), static_cast<int>(point_lights.size()));

        // Upload each directional light to shader
        for (size_t i = 0; i < dir_lights.size(); i++) {
            dir_lights[i]->upload_to_shader(shader_program, i);
        }

        // Upload each point light to shader
        for (size_t i = 0; i < point_lights.size(); i++) {
            point_lights[i]->upload_to_shader(shader_program, i);
        }
    }

    void Renderer::render(Object3D &scene, Camera &camera) {
        // Collect all lights in the scene
        std::vector<DirectionalLight *> dir_lights;
        std::vector<PointLight *> point_lights;
        collect_lights(&scene, dir_lights, point_lights);

        // Activate shader
        glUseProgram(program_id_);

        // Set view/projection matrices
        glm::mat4 view = camera.get_view_matrix();
        glm::mat4 projection = camera.get_projection_matrix();

        // Pass camera position for specular calculation
        glUniform3fv(glGetUniformLocation(program_id_, "viewPos"), 1, glm::value_ptr(camera.get_position()));

        // Upload all the lights to the shader
        upload_lights(program_id_, dir_lights, point_lights);

        // Draw the scene
        scene.draw(view, projection, program_id_, context_);
    }

    uint32_t Renderer::get_default_shader() const {
        return program_id_;
    }

    void *Renderer::get_context() const {
        return context_;
    }

    void Renderer::begin_frame() {
        // Setup frame
        glClearColor(0.1f, 0.2f, 0.3f, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::end_frame() {
    }
}
