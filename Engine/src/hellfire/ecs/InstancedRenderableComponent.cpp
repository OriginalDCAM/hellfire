// InstancedRenderableComponent.cpp
#include "hellfire/ecs/InstancedRenderableComponent.h"
#include <GL/glew.h>

namespace hellfire {
    InstancedRenderableComponent::InstancedRenderableComponent(
        std::shared_ptr<Mesh> mesh, size_t max_instances)
        : mesh_(std::move(mesh)), max_instances_(max_instances),
          needs_gpu_update_(false), instance_vbo_(0),
          transform_buffer_(0), color_buffer_(0), scale_buffer_(0) {
        instances_.reserve(max_instances_);
        setup_instance_buffers();
    }

    InstancedRenderableComponent::~InstancedRenderableComponent() {
        cleanup_buffers();
    }

    void InstancedRenderableComponent::add_instance(const InstanceData& instance) {
        if (instances_.size() < max_instances_) {
            instances_.push_back(instance);
            needs_gpu_update_ = true;
        }
    }

    void InstancedRenderableComponent::update_instance(size_t index, const InstanceData& instance) {
        if (index < instances_.size()) {
            instances_[index] = instance;
            needs_gpu_update_ = true;
        }
    }

    void InstancedRenderableComponent::clear_instances() {
        instances_.clear();
        needs_gpu_update_ = true;
    }

    void InstancedRenderableComponent::reserve_instances(size_t count) {
        instances_.reserve((std::min)(count, max_instances_));
    }

    void InstancedRenderableComponent::set_instances(const std::vector<InstanceData>& instances) {
        instances_ = instances;
        if (instances_.size() > max_instances_) {
            instances_.resize(max_instances_);
        }
        needs_gpu_update_ = true;
    }

    void InstancedRenderableComponent::add_instances(const std::vector<InstanceData>& instances) {
        for (const auto& instance : instances) {
            add_instance(instance);
        }
    }

    void InstancedRenderableComponent::prepare_for_draw() {
        if (needs_gpu_update_) {
            update_gpu_buffer();
            needs_gpu_update_ = false;
        }
    }

    void InstancedRenderableComponent::bind_instance_buffers() {
        setup_instanced_vertex_attributes();
        enable_instance_attributes();
    }

    void InstancedRenderableComponent::unbind_instance_buffers() {
        disable_instance_attributes();
    }

    void InstancedRenderableComponent::setup_instance_buffers() {
        glGenBuffers(1, &instance_vbo_);
        glGenBuffers(1, &transform_buffer_);
        glGenBuffers(1, &color_buffer_);
        glGenBuffers(1, &scale_buffer_);
    }

    void InstancedRenderableComponent::cleanup_buffers() {
        if (instance_vbo_) glDeleteBuffers(1, &instance_vbo_);
        if (transform_buffer_) glDeleteBuffers(1, &transform_buffer_);
        if (color_buffer_) glDeleteBuffers(1, &color_buffer_);
        if (scale_buffer_) glDeleteBuffers(1, &scale_buffer_);
        
        instance_vbo_ = 0;
        transform_buffer_ = 0;
        color_buffer_ = 0;
        scale_buffer_ = 0;
    }

    void InstancedRenderableComponent::update_gpu_buffer() {
        if (instances_.empty()) return;

        std::vector<glm::mat4> transforms;
        std::vector<glm::vec3> colors;
        std::vector<float> scales;

        transforms.reserve(instances_.size());
        colors.reserve(instances_.size());
        scales.reserve(instances_.size());

        for (const auto& instance : instances_) {
            transforms.push_back(instance.transform);
            colors.push_back(instance.color);
            scales.push_back(instance.scale);
        }

        // Upload transforms (layout 4-7)
        glBindBuffer(GL_ARRAY_BUFFER, transform_buffer_);
        glBufferData(GL_ARRAY_BUFFER, transforms.size() * sizeof(glm::mat4),
                     transforms.data(), GL_DYNAMIC_DRAW);

        // Upload colors (layout 8)
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3),
                     colors.data(), GL_DYNAMIC_DRAW);

        // Upload scales (layout 9)
        glBindBuffer(GL_ARRAY_BUFFER, scale_buffer_);
        glBufferData(GL_ARRAY_BUFFER, scales.size() * sizeof(float),
                     scales.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void InstancedRenderableComponent::setup_instanced_vertex_attributes() {
        if (vertex_attributes_setup_) return;

        // Transform matrix (layouts 4-7)
        glBindBuffer(GL_ARRAY_BUFFER, transform_buffer_);
        for (int i = 0; i < 4; i++) {
            glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                                (void*)(sizeof(glm::vec4) * i));
            glVertexAttribDivisor(4 + i, 1);
        }

        // Color (layout 8)
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
        glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glVertexAttribDivisor(8, 1);

        // Scale (layout 9)
        glBindBuffer(GL_ARRAY_BUFFER, scale_buffer_);
        glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
        glVertexAttribDivisor(9, 1);

        vertex_attributes_setup_ = true;
    }

    void InstancedRenderableComponent::enable_instance_attributes() {
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);
        glEnableVertexAttribArray(7);
        glEnableVertexAttribArray(8);
        glEnableVertexAttribArray(9);
    }

    void InstancedRenderableComponent::disable_instance_attributes() {
        glDisableVertexAttribArray(4);
        glDisableVertexAttribArray(5);
        glDisableVertexAttribArray(6);
        glDisableVertexAttribArray(7);
        glDisableVertexAttribArray(8);
        glDisableVertexAttribArray(9);
    }
}