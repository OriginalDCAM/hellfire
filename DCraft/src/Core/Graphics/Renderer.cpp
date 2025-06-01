//
// Simplified Renderer.cpp
//
#include "DCraft/Graphics/Renderer.h"
#include "GL/glew.h"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

namespace DCraft {
    Renderer::Renderer(uint32_t fallback_program_id) : fallback_program_id_(fallback_program_id),
                                                       render_to_framebuffer_(false), framebuffer_width_(800),
                                                       framebuffer_height_(600) {
        OGLRendererContext *ogl_context = new OGLRendererContext();
        ogl_context->default_shader_program = fallback_program_id;
        context_ = static_cast<void *>(ogl_context);
    }

    Renderer::Renderer()
        : fallback_program_id_(0), render_to_framebuffer_(false),
          framebuffer_width_(800), framebuffer_height_(600) {
        OGLRendererContext *ogl_context = new OGLRendererContext();
        ogl_context->default_shader_program = 0;
        context_ = static_cast<void *>(ogl_context);
    }

    Renderer::~Renderer() {
        if (context_) {
            OGLRendererContext *ogl_context = static_cast<OGLRendererContext *>(context_);
            delete ogl_context;
            context_ = nullptr;
        }
    }

    void Renderer::init() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthFunc(GL_LESS);
        glDisable(GL_CULL_FACE);
    }

    void Renderer::render(Object3D &scene, Camera &camera) {
        if (render_to_framebuffer_) {
            render_scene_to_framebuffer(scene, camera);
        } else {
            begin_frame();
            render_internal(scene, camera);
            end_frame();
        }
    }

    void Renderer::begin_frame() {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        opaque_objects_.clear();
        transparent_objects_.clear();
    }

    void Renderer::end_frame() {
        glFlush();
    }

    uint32_t Renderer::get_default_shader() const {
        return fallback_program_id_;
    }

    void *Renderer::get_context() const {
        return context_;
    }

    void Renderer::store_lights_in_context(const std::vector<DirectionalLight *> &dir_lights,
                                           const std::vector<PointLight *> &point_lights,
                                           Camera &camera) {
        OGLRendererContext *ogl_context = static_cast<OGLRendererContext *>(context_);
        if (!ogl_context) return;

        // Store directional lights
        ogl_context->num_directional_lights = std::min(static_cast<int>(dir_lights.size()), 4);
        for (int i = 0; i < ogl_context->num_directional_lights; i++) {
            ogl_context->directional_lights[i] = dir_lights[i];
        }

        // Store point lights
        ogl_context->num_point_lights = std::min(static_cast<int>(point_lights.size()), 8);
        for (int i = 0; i < ogl_context->num_point_lights; i++) {
            ogl_context->point_lights[i] = point_lights[i];
        }

        // Store camera
        ogl_context->camera = &camera;
    }

    void Renderer::render_internal(Object3D &scene, Camera &camera) {
        opaque_objects_.clear();
        transparent_objects_.clear();
        
        // 1. Collect light data and store in render context
        std::vector<DirectionalLight *> dir_lights;
        std::vector<PointLight *> point_lights;
        collect_lights(&scene, dir_lights, point_lights);
        store_lights_in_context(dir_lights, point_lights, camera);

        // 2. Collect render commands
        collect_render_commands(&scene, camera.get_position());

        // 3. Render in two passes
        glm::mat4 view = camera.get_view_matrix();
        glm::mat4 projection = camera.get_projection_matrix();

        render_opaque_pass(view, projection);
        render_transparent_pass(view, projection);
    }

    void Renderer::collect_lights(Object3D *object_3d, std::vector<DirectionalLight *> &dir_lights,
                                  std::vector<PointLight *> &point_lights) {
        if (auto *dir_light = dynamic_cast<DirectionalLight *>(object_3d)) {
            dir_lights.push_back(dir_light);
        } else if (auto *point_light = dynamic_cast<PointLight *>(object_3d)) {
            point_lights.push_back(point_light);
        }

        for (auto *child: object_3d->get_children()) {
            collect_lights(child, dir_lights, point_lights);
        }
    }

    void Renderer::collect_render_commands(Object3D *object, const glm::vec3 &camera_pos) {
        if (object->has_mesh()) {
            Mesh *mesh = object->get_mesh();
            if (mesh && mesh->get_material()) {
                Material *material = mesh->get_material();

                glm::vec3 object_pos = object->get_world_position();
                float distance = glm::length(camera_pos - object_pos);
                bool is_transparent = is_material_transparent(material);

                RenderCommand cmd = {object, material, distance, is_transparent};

                if (is_transparent) {
                    transparent_objects_.push_back(cmd);
                } else {
                    opaque_objects_.push_back(cmd);
                }
            }
        }

        for (auto *child: object->get_children()) {
            collect_render_commands(child, camera_pos);
        }
    }

    bool Renderer::is_material_transparent(Material *material) {
        float transparency = material->get_property<float>("transparency", 1.0f);
        float alpha = material->get_property<float>("alpha", 1.0f);
        bool use_transparency = material->get_property<bool>("useTransparency", false);
        return transparency < 1.0f || alpha < 1.0f || use_transparency;
    }

    void Renderer::render_opaque_pass(const glm::mat4 &view, const glm::mat4 &projection) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        std::sort(opaque_objects_.begin(), opaque_objects_.end(),
                  [](const RenderCommand &a, const RenderCommand &b) {
                      return a.distance_to_camera < b.distance_to_camera;
                  });

        for (const auto &cmd: opaque_objects_) {
            cmd.object->draw_self(view, projection, fallback_program_id_, context_);
        }
    }

    void Renderer::render_transparent_pass(const glm::mat4 &view, const glm::mat4 &projection) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        std::sort(transparent_objects_.begin(), transparent_objects_.end(),
                  [](const RenderCommand &a, const RenderCommand &b) {
                      return a.distance_to_camera > b.distance_to_camera;
                  });

        for (const auto &cmd: transparent_objects_) {
            cmd.object->draw_self(view, projection, fallback_program_id_, context_);
        }

        glDepthMask(GL_TRUE);
    }

    void Renderer::create_scene_framebuffer(uint32_t width, uint32_t height) {
        framebuffer_width_ = width;
        framebuffer_height_ = height;
        scene_framebuffer_ = std::make_unique<Framebuffer>(width, height);
    }

    void Renderer::resize_scene_framebuffer(uint32_t width, uint32_t height) {
        if (scene_framebuffer_) {
            framebuffer_width_ = width;
            framebuffer_height_ = height;
            scene_framebuffer_->resize(width, height);
        } else {
            create_scene_framebuffer(width, height);
        }
    }

    uint32_t Renderer::get_scene_texture() const {
        if (scene_framebuffer_) {
            return scene_framebuffer_->get_color_texture();
        }
        return 0;
    }

    void Renderer::render_to_texture(Object3D &scene, Camera &camera, uint32_t width, uint32_t height) {
        if (!scene_framebuffer_ || scene_framebuffer_->get_width() != width || scene_framebuffer_->get_height() !=
            height) {
            resize_scene_framebuffer(width, height);
        }

        render_scene_to_framebuffer(scene, camera);
    }

    void Renderer::render_scene_to_framebuffer(Object3D &scene, Camera &camera) {
        if (!scene_framebuffer_) {
            create_scene_framebuffer(framebuffer_width_, framebuffer_height_);
        }

        scene_framebuffer_->bind();

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);

        render_internal(scene, camera);

        scene_framebuffer_->unbind();

        glFlush();
    }

    void Renderer::set_fallback_shader(uint32_t shader_program_id) {
        fallback_program_id_ = shader_program_id;

        if (context_) {
            OGLRendererContext *ogl_context = static_cast<OGLRendererContext *>(context_);
            ogl_context->default_shader_program = shader_program_id;
        }
    }
}
