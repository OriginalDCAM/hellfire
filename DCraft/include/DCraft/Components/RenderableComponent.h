#pragma once
#include <memory>
#include <glm/glm.hpp>

#include "DCraft/Structs/Component.h"
#include "DCraft/Components/TransformComponent.h"
#include "DCraft/Graphics/Mesh.h"
#include "DCraft/Graphics/Materials/MaterialRenderer.h"
#include "DCraft/Graphics/RenderingUtils.h"

namespace DCraft {
    class RenderableComponent : public Component {
    public:
        RenderableComponent() = default;

        RenderableComponent(std::shared_ptr<Mesh> mesh) : mesh_(mesh) {
        }

        // Mesh management
        void set_mesh(std::shared_ptr<Mesh> mesh) { mesh_ = mesh; }
        void set_mesh(Mesh* mesh) { 
            if (mesh) {
                mesh_ = std::shared_ptr<Mesh>(mesh);
            } else {
                mesh_.reset();
            }
        }
        Mesh* get_mesh() const { return mesh_.get(); }

        std::shared_ptr<Mesh> get_mesh_shared() const { return mesh_; }
        bool has_mesh() const { return mesh_ != nullptr; }

        // Material convenience methods
        void set_material(Material* material) {
            if (material && has_mesh()) {
                get_mesh()->set_material(material);
            }
        }

        Material* get_material() const {
            return has_mesh() ? get_mesh()->get_material() : nullptr;
        }

        /// New main rendering method
        void draw(const glm::mat4& view, const glm::mat4& projection, 
          Shader& shader, void* renderer_context = nullptr, float time = 0.0f) {
            if (!has_mesh() || !get_owner()) return;

            Mesh* mesh = get_mesh();
            if (!RenderingUtils::validate_mesh_for_rendering(mesh)) return;

            auto* transform = get_owner()->get_component<TransformComponent>();
            if (!transform) return;

            glm::mat4 model = transform->get_world_matrix();

            shader.use();
        
            if (renderer_context) {
                RenderingUtils::upload_lights_to_shader(shader, renderer_context);
            }

            RenderingUtils::set_standard_uniforms(shader, model, view, projection, time);

            MaterialRenderer::bind_material(*mesh->get_material());
            mesh->draw();
        }
    private:
        std::shared_ptr<Mesh> mesh_;
    };
}
