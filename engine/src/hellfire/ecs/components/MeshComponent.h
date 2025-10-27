//
// Created by denzel on 29/09/2025.
//

#pragma once
#include "hellfire/ecs/Component.h"
#include "hellfire/graphics/Mesh.h"

namespace hellfire {
    enum class MeshSource {
        INTERNAL,
        EXTERNAL
    };
    
    class MeshComponent final : public Component {
    public:
        MeshComponent() = default;
        explicit MeshComponent(std::shared_ptr<Mesh> mesh) : mesh_(std::move(mesh)) {}

        void set_mesh(std::shared_ptr<Mesh> mesh) { mesh_ = std::move(mesh); }
        [[nodiscard]] std::shared_ptr<Mesh> get_mesh() const { return mesh_; }
        [[nodiscard]] bool has_mesh() const { return mesh_ != nullptr; }

        void set_source(const MeshSource source) { source_ = source; }
        MeshSource get_source() const { return source_;}
    private:
        std::shared_ptr<Mesh> mesh_;
        MeshSource source_ = MeshSource::EXTERNAL;

    };
}
