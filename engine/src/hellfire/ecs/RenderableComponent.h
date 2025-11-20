#pragma once
#include <memory>

#include "Component.h"
#include "hellfire/graphics/managers/MaterialManager.h"

namespace hellfire {
    /// Renderable Component used for single mesh rendering
    class RenderableComponent final : public Component {
    public:
        RenderableComponent() = default;

        // Material management
        void set_material(const std::shared_ptr<Material>& material) { material_ = material; }
        [[nodiscard]] std::shared_ptr<Material> get_material() const { return material_; }
        [[nodiscard]] bool has_material() const { return material_ != nullptr; }

        // Rendering settings
        void set_cast_shadows(bool cast) { cast_shadows_ = cast; }
        void set_receive_shadows(bool receive) { receive_shadows_ = receive; }
        [[nodiscard]] bool get_cast_shadows() const { return cast_shadows_; }
        [[nodiscard]] bool get_receive_shadows() const { return receive_shadows_; }
    private:
        std::shared_ptr<Material> material_;
        bool cast_shadows_ = true;
        bool receive_shadows_ = true;
    };
}
