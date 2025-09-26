#pragma once
#include <memory>
#include <utility>

#include "Component.h"
#include "hellfire/ecs/TransformComponent.h"
#include "hellfire/graphics/Mesh.h"
#include "hellfire/graphics/managers/MaterialManager.h"
#include "../graphics/renderer/RenderingUtils.h"

namespace hellfire {
    /// Renderable Component used for single mesh rendering
    class RenderableComponent final : public Component {
    public:
        RenderableComponent() = default;

        explicit RenderableComponent(std::shared_ptr<Mesh> mesh) : mesh_(std::move(mesh)) {}

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

        [[nodiscard]] std::shared_ptr<Mesh> get_mesh_shared() const { return mesh_; }
        [[nodiscard]] bool has_mesh() const { return mesh_ != nullptr; }

        void set_material(std::shared_ptr<Material> material) const {
            if (material && has_mesh()) {
                get_mesh()->set_material(material);
            }
        }

        [[nodiscard]] std::shared_ptr<Material> get_material() const {
            return has_mesh() ? get_mesh()->get_material() : nullptr;
        }

        /// New main rendering method
        void draw(const glm::mat4 &view, const glm::mat4 &projection,
                  Shader &shader, void *renderer_context = nullptr, float time = 0.0f) const {
            if (!has_mesh() || !get_owner()) return;

            Mesh *mesh = get_mesh();
            if (!RenderingUtils::validate_mesh_for_rendering(mesh)) return;

            auto material = mesh->get_material();
            if (!material) return;

            const auto *transform = get_owner()->get_component<TransformComponent>();
            if (!transform) return;

            const glm::mat4 model = transform->get_world_matrix();


            // Abstract this away
            uint32_t shader_id = material->get_compiled_shader_id();
            glUseProgram(shader_id);
            auto material_shader = Shader::from_id(shader_id);
            
            if (renderer_context) {
                RenderingUtils::upload_lights_to_shader(material_shader, renderer_context);
            }

            RenderingUtils::set_standard_uniforms(material_shader, model, view, projection, time);

            material->bind();
            mesh->draw();
            material->unbind();
        }

    private:
        std::shared_ptr<Mesh> mesh_;
    };
}
