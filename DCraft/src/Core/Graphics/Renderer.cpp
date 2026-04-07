#include "DCraft/Graphics/Renderer.h"
#include "GL/glew.h"
#include <glm/gtc/type_ptr.hpp>  // Fixed: was .inl
#include <unordered_set>

namespace DCraft {
    class PointLight;

    Renderer::Renderer(uint32_t fallback_program_id) : fallback_program_id_(fallback_program_id) {
        OGLRendererContext *ogl_context = new OGLRendererContext();
        ogl_context->default_shader_program = fallback_program_id;
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

    void Renderer::collect_unique_shaders(Object3D* object, std::unordered_set<uint32_t>& shaders) {
        // Check if this object has a material with a shader
        if (object->has_mesh()) {
            Mesh* mesh = object->get_mesh();
            if (mesh && mesh->get_material()) {
                uint32_t shader_id = mesh->get_material()->get_compiled_shader_id();
                if (shader_id != 0) {
                    shaders.insert(shader_id);
                }
            }
        }

        // Check children
        for (auto* child : object->get_children()) {
            collect_unique_shaders(child, shaders);
        }
    }

    void Renderer::upload_lights_to_shader(uint32_t shader_program, 
                                          const std::vector<DirectionalLight *> &dir_lights,
                                          const std::vector<PointLight *> &point_lights) {
        glUseProgram(shader_program);

        // Upload light counts
        GLint dir_count_loc = glGetUniformLocation(shader_program, "numDirectionalLights");
        if (dir_count_loc != -1) {
            glUniform1i(dir_count_loc, static_cast<int>(std::min(dir_lights.size(), size_t(4)))); // MAX_DIRECTIONAL_LIGHTS = 4
        }

        GLint point_count_loc = glGetUniformLocation(shader_program, "numPointLights");
        if (point_count_loc != -1) {
            glUniform1i(point_count_loc, static_cast<int>(std::min(point_lights.size(), size_t(8)))); // MAX_POINT_LIGHTS = 8
        }

        // Upload each directional light to shader
        for (size_t i = 0; i < std::min(dir_lights.size(), size_t(4)); i++) {
            dir_lights[i]->upload_to_shader(shader_program, i);
        }

        // Upload each point light to shader
        for (size_t i = 0; i < std::min(point_lights.size(), size_t(8)); i++) {
            point_lights[i]->upload_to_shader(shader_program, i);
        }
    }

    void Renderer::upload_global_uniforms_to_shader(uint32_t shader_program, Camera& camera) {
        glUseProgram(shader_program);

        // Pass camera position for specular calculation
        GLint view_pos_loc = glGetUniformLocation(shader_program, "viewPos");
        if (view_pos_loc != -1) {
            glUniform3fv(view_pos_loc, 1, glm::value_ptr(camera.get_position()));
        }

        // You can add other global uniforms here like:
        // - Time
        // - Screen resolution
        // - Global fog settings
        // etc.
    }

    void Renderer::render(Object3D &scene, Camera &camera) {
        // Collect all lights in the scene
        std::vector<DirectionalLight *> dir_lights;
        std::vector<PointLight *> point_lights;
        collect_lights(&scene, dir_lights, point_lights);

        // Collect all unique shaders used in the scene
        std::unordered_set<uint32_t> unique_shaders;
        collect_unique_shaders(&scene, unique_shaders);

        // Add fallback shader if no materials have shaders
        if (unique_shaders.empty()) {
            unique_shaders.insert(fallback_program_id_);
        }

        // Upload lights and global uniforms to all shaders that will be used
        for (uint32_t shader_id : unique_shaders) {
            if (shader_id != 0) {
                upload_lights_to_shader(shader_id, dir_lights, point_lights);
                upload_global_uniforms_to_shader(shader_id, camera);
            }
        }

        // Set view/projection matrices (these will be set per-object in draw calls)
        glm::mat4 view = camera.get_view_matrix();
        glm::mat4 projection = camera.get_projection_matrix();

        // Draw the scene - each object will use its material's shader
        scene.draw(view, projection, fallback_program_id_, context_);
    }

    uint32_t Renderer::get_default_shader() const {
        return fallback_program_id_;
    }

    void *Renderer::get_context() const {
        return context_;
    }

    void Renderer::begin_frame() {
        // Setup frame
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::end_frame() {
        // End frame cleanup if needed
    }
}