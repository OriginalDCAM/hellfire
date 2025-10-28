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

#include "hellfire/graphics/Mesh.h"
#include "hellfire/graphics/renderer/RenderingUtils.h"
#include "hellfire/graphics/managers/MaterialManager.h"
#include "Component.h"

#define Color glm::vec3
#define Transform glm::mat4

namespace hellfire {
    class Mesh;
    class Material;

    class InstancedRenderableComponent : public Component {
    public:
        struct InstanceData {
            glm::mat4 transform;
            glm::vec3 color;
            float scale;

            explicit InstanceData(const glm::mat4& t = glm::mat4(1.0f),
                                const glm::vec3& c = glm::vec3(1.0f), 
                                float s = 1.0f) 
                : transform(t), color(c), scale(s) {}
        };

        InstancedRenderableComponent() = default;
        explicit InstancedRenderableComponent(std::shared_ptr<Mesh> mesh, size_t max_instances = 1000);
        ~InstancedRenderableComponent() override;

        // Mesh management (kept for now, but prefer using MeshComponent)
        void set_mesh(std::shared_ptr<Mesh> mesh) { mesh_ = mesh; }
        [[nodiscard]] std::shared_ptr<Mesh> get_mesh() const { return mesh_; }
        [[nodiscard]] bool has_mesh() const { return mesh_ != nullptr; }

        // Material management - NO LONGER stores on mesh
        void set_material(std::shared_ptr<Material> material) { material_ = material; }
        [[nodiscard]] std::shared_ptr<Material> get_material() const { return material_; }

        // Instance management
        void add_instance(const InstanceData& instance);
        void update_instance(size_t index, const InstanceData& instance);
        void clear_instances();
        void reserve_instances(size_t count);
        void set_instances(const std::vector<InstanceData>& instances);
        void add_instances(const std::vector<InstanceData>& instances);

        // Getters
        size_t get_instance_count() const { return instances_.size(); }
        size_t get_max_instances() const { return max_instances_; }
        const std::vector<InstanceData>& get_instances() const { return instances_; }

        // Rendering - called by Renderer, not by component itself
        void prepare_for_draw();
        void bind_instance_buffers();
        void unbind_instance_buffers();

    private:
        std::shared_ptr<Mesh> mesh_;
        std::shared_ptr<Material> material_;  // Store material here, not on mesh
        std::vector<InstanceData> instances_;
        size_t max_instances_;
        bool needs_gpu_update_;
        bool vertex_attributes_setup_ = false;

        // OpenGL buffers
        GLuint instance_vbo_;
        GLuint transform_buffer_;
        GLuint color_buffer_;
        GLuint scale_buffer_;

        void setup_instance_buffers();
        void cleanup_buffers();
        void update_gpu_buffer() const;
        void setup_instanced_vertex_attributes();
        void enable_instance_attributes();
        void disable_instance_attributes();
    };
};
