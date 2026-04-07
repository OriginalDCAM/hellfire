//
// Created by denzel on 29/09/2025.
//

#pragma once
#include "hellfire/ecs/Component.h"
#include "hellfire/graphics/Mesh.h"
#include "hellfire/assets/AssetRegistry.h"

namespace hellfire {
    enum class MeshSource {
        INTERNAL,
        EXTERNAL
    };

    enum class MeshInternalType : uint8_t {
        NONE = 0,
        CUBE = 1,
        SPHERE = 2,
        QUAD = 3
    };
    
    class MeshComponent final : public Component {
    public:
        MeshComponent() = default;
        explicit MeshComponent(std::shared_ptr<Mesh> mesh) : mesh_(std::move(mesh)) {}

        void set_mesh(std::shared_ptr<Mesh> mesh) { mesh_ = std::move(mesh); }
        [[nodiscard]] std::shared_ptr<Mesh> get_mesh() const { return mesh_; }
        [[nodiscard]] bool has_mesh() const { return mesh_ != nullptr; }

        void set_mesh_asset(AssetID id) { mesh_asset_id_ = id; }
    
        AssetID get_mesh_asset() const { return mesh_asset_id_; }

        void set_source(const MeshSource source, MeshInternalType type = MeshInternalType::NONE) {
            source_ = source;
            internal_type = type;
        }
        MeshSource get_source() const { return source_;}

        bool is_wireframe = false;
    private:
        std::shared_ptr<Mesh> mesh_;
        MeshSource source_ = MeshSource::EXTERNAL;
        MeshInternalType internal_type = MeshInternalType::NONE;
        AssetID mesh_asset_id_ = INVALID_ASSET_ID; 
    };
}
