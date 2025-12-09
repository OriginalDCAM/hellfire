#pragma once
#include <memory>

#include "Component.h"
#include "hellfire/assets/AssetRegistry.h"
#include "hellfire/graphics/managers/MaterialManager.h"

namespace hellfire {
    /// Renderable Component used for single mesh rendering
    class RenderableComponent final : public Component {
    public:
        RenderableComponent() = default;

        // Material management
        void set_material(const std::shared_ptr<Material>& material) { material_ = material; }
        void set_material_asset(AssetID id) { material_asset_id_ = id; }
        
        [[nodiscard]] std::shared_ptr<Material> get_material() const { return material_; }
        AssetID get_material_asset() const { return material_asset_id_; }
        [[nodiscard]] bool has_material() const { return material_ != nullptr; }

        // Rendering settings
        void set_cast_shadows(bool cast) { cast_shadows = cast; }
        void set_receive_shadows(bool receive) { receive_shadows = receive; }
        [[nodiscard]] bool get_cast_shadows() const { return cast_shadows; }
        [[nodiscard]] bool get_receive_shadows() const { return receive_shadows; }

        // Render settings
        bool cast_shadows = true;
        bool receive_shadows = true;
        bool visible = true;
        uint32_t render_layer = 0;
    private:
        std::shared_ptr<Material> material_;
        AssetID material_asset_id_ = INVALID_ASSET_ID;
        

    };
}
