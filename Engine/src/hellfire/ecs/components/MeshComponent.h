//
// Created by denzel on 29/09/2025.
//

#pragma once
#include "hellfire/ecs/Component.h"
#include "hellfire/graphics/Mesh.h"

namespace hellfire {
    class MeshComponent final : public Component {
    public:
        MeshComponent() = default;
        explicit MeshComponent(std::shared_ptr<Mesh> mesh) : mesh_(std::move(mesh)) {}

        void set_mesh(std::shared_ptr<Mesh> mesh) { mesh_ = std::move(mesh); }
        [[nodiscard]] std::shared_ptr<Mesh> get_mesh() const { return mesh_; }
        [[nodiscard]] bool has_mesh() const { return mesh_ != nullptr; }

    private:
        std::shared_ptr<Mesh> mesh_;
    };
}
