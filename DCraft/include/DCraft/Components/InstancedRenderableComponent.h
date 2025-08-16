//
// Created by denzel on 14/08/2025.
//
#pragma once
#include <memory>
#include <utility>
#include <vector>
#include <GL/glew.h>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/detail/type_vec3.hpp>

#include "DCraft/Graphics/Mesh.h"
#include "DCraft/Graphics/RenderingUtils.h"
#include "DCraft/Graphics/Materials/MaterialRenderer.h"
#include "DCraft/Structs/Component.h"

#define Color glm::vec3
#define Transform glm::mat4

namespace DCraft {
    struct Mesh;

    class InstancedRenderableComponent : public Component {
    public:
        struct InstanceData {
            Transform transform;
            Color color;
            float scale;

            InstanceData(const Transform &t = Transform(1.0f),
                         const Color &c = Color(1.0f), float s = 1.0f) : transform(t), color(c), scale(s) {
            }
        };

        InstancedRenderableComponent() = default;

        explicit InstancedRenderableComponent(std::shared_ptr<Mesh> mesh, size_t max_instances = 1000)
            : mesh_(std::move(mesh)), max_instances_(max_instances), needs_gpu_update_(false),
              instance_vbo_(0), transform_buffer_(0), color_buffer_(0), scale_buffer_(0) {
            instances_.reserve(max_instances_);
            setup_instance_buffers();
        }

        virtual ~InstancedRenderableComponent() {
            cleanup_buffers();
        }

        // Mesh management
        void set_mesh(const std::shared_ptr<Mesh> &mesh) { mesh_ = mesh; }

        void set_mesh(Mesh *mesh) {
            if (mesh) {
                mesh_ = std::shared_ptr<Mesh>(mesh);
            } else {
                mesh_.reset();
            }
        }

        [[nodiscard]] Mesh *get_mesh() const { return mesh_.get(); }
        [[nodiscard]] bool has_mesh() const { return mesh_ != nullptr; }

        void set_material(Material *material) const {
            if (material && has_mesh()) {
                get_mesh()->set_material(material);
            }
        }

        [[nodiscard]] Material *get_material() const {
            return has_mesh() ? get_mesh()->get_material() : nullptr;
        }

        // Instance management
        void add_instance(const InstanceData &instance) {
            if (instances_.size() < max_instances_) {
                instances_.push_back(instance);
                needs_gpu_update_ = true;
            }
        }

        void update_instance(size_t index, const InstanceData &instance) {
            if (index < instances_.size()) {
                instances_[index] = instance;
                needs_gpu_update_ = true;
            }
        }

        void clear_instances() {
            instances_.clear();
            needs_gpu_update_ = true;
        }

        void reserve_instances(size_t count) {
            instances_.reserve(std::min(count, max_instances_));
        }

        // Getters
        size_t get_instance_count() const { return instances_.size(); }
        size_t get_max_instances() const { return max_instances_; }
        const std::vector<InstanceData> &get_instances() const { return instances_; }
        std::vector<InstanceData> &get_instances() { return instances_; } // Non-const for updates

        // Bulk operations
        void set_instances(const std::vector<InstanceData> &instances) {
            instances_ = instances;
            if (instances_.size() > max_instances_) {
                instances_.resize(max_instances_);
            }
            needs_gpu_update_ = true;
        }

        void add_instances(const std::vector<InstanceData> &instances) {
            for (const auto &instance: instances) {
                add_instance(instance);
            }
        }

        /// Main rendering method (consistent with RenderableComponent interface)
        void draw(const glm::mat4 &view, const glm::mat4 &projection,
                  Shader &shader, void *renderer_context = nullptr, float time = 0.0f) {
            if (!has_mesh() || instances_.empty()) return;

            Mesh *mesh = get_mesh();
            if (!RenderingUtils::validate_mesh_for_rendering(mesh)) return;

            // Update GPU buffers if needed
            if (needs_gpu_update_) {
                update_gpu_buffer();
                needs_gpu_update_ = false;
            }

            shader.use();

            if (renderer_context) {
                RenderingUtils::upload_lights_to_shader(shader, renderer_context);
            }

            RenderingUtils::set_standard_uniforms(shader, glm::mat4(1.0f), view, projection, time);

            // Bind material
            MaterialRenderer::bind_material(*mesh->get_material());

            mesh->bind();
            setup_instanced_vertex_attributes();
            enable_instance_attributes();

            // Draw all instances
            mesh->draw_instanced(instances_.size());

            // Cleanup
            disable_instance_attributes();
            mesh->unbind();
        }

    private:
        std::shared_ptr<Mesh> mesh_;
        std::vector<InstanceData> instances_;
        size_t max_instances_;
        bool needs_gpu_update_;
        bool vertex_attributes_setup_ = false;

        // OpenGL buffers
        GLuint instance_vbo_;
        GLuint transform_buffer_;
        GLuint color_buffer_;
        GLuint scale_buffer_;

        void setup_instanced_vertex_attributes() {
            if (vertex_attributes_setup_) return;
            // Bind transform buffer and setup mat4 attribute (layouts 4-7)
            glBindBuffer(GL_ARRAY_BUFFER, transform_buffer_);
            for (int i = 0; i < 4; i++) {
                glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), 
                                    (void*)(sizeof(glm::vec4) * i));
                glVertexAttribDivisor(4 + i, 1);
            }
    
            // Bind color buffer (layout 8)
            glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
            glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
            glVertexAttribDivisor(8, 1);
    
            // Bind scale buffer (layout 9)
            glBindBuffer(GL_ARRAY_BUFFER, scale_buffer_);
            glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
            glVertexAttribDivisor(9, 1);

            vertex_attributes_setup_ = true;
        }

        void setup_instance_buffers() {
            // Implementation details for setting up instanced rendering buffers
            glGenBuffers(1, &instance_vbo_);
            glGenBuffers(1, &transform_buffer_);
            glGenBuffers(1, &color_buffer_);
            glGenBuffers(1, &scale_buffer_);
        }

        void cleanup_buffers() {
            if (instance_vbo_)
                glDeleteBuffers(1, &instance_vbo_);
            if (transform_buffer_)
                glDeleteBuffers(1, &transform_buffer_);
            if (color_buffer_)
                glDeleteBuffers(1, &color_buffer_);
            if (scale_buffer_)
                glDeleteBuffers(1, &scale_buffer_);
        }

        void update_gpu_buffer() {
            if (instances_.empty()) return;

            // Prepare data arrays for efficient upload
            std::vector<Transform> transforms;
            std::vector<Color> colors;
            std::vector<float> scales;

            transforms.reserve(instances_.size());
            colors.reserve(instances_.size());
            scales.reserve(instances_.size());

            // Extract data from instances
            for (const auto &instance: instances_) {
                transforms.push_back(instance.transform);
                colors.push_back(instance.color);
                scales.push_back(instance.scale);
            }

            // Upload transform matrices to GPU (layout 4-7)
            glBindBuffer(GL_ARRAY_BUFFER, transform_buffer_);
            glBufferData(GL_ARRAY_BUFFER,
                         transforms.size() * sizeof(Transform),
                         transforms.data(),
                         GL_DYNAMIC_DRAW);

            // Upload instance colors to GPU (layout 8)
            glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
            glBufferData(GL_ARRAY_BUFFER,
                         colors.size() * sizeof(Color),
                         colors.data(),
                         GL_DYNAMIC_DRAW);

            // Upload instance scales to GPU (layout 9)
            glBindBuffer(GL_ARRAY_BUFFER, scale_buffer_);
            glBufferData(GL_ARRAY_BUFFER,
                         scales.size() * sizeof(float),
                         scales.data(),
                         GL_DYNAMIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        void enable_instance_attributes() {
            // Enable vertex attributes for instanced data
            glEnableVertexAttribArray(4); // Transform matrix (uses slots 3,4,5,6)
            glEnableVertexAttribArray(5);
            glEnableVertexAttribArray(6);
            glEnableVertexAttribArray(7); // Color
            glEnableVertexAttribArray(8); // Scale
            glEnableVertexAttribArray(9);
        }

        void disable_instance_attributes() {
            glDisableVertexAttribArray(4);
            glDisableVertexAttribArray(5);
            glDisableVertexAttribArray(6);
            glDisableVertexAttribArray(7);
            glDisableVertexAttribArray(8);
            glDisableVertexAttribArray(9);
        }
    };
};
