//
// Created by denzel on 30/11/2025.
//

#include "ComponentRegistration.h"

#include "ComponentRegistry.h"
#include "TransformComponent.h"

namespace hellfire {
    void register_all_components() {
        auto &reg = ComponentRegistry::instance();
        reg.register_component<TransformComponent>("TransformComponent");
        reg.register_component<MeshComponent>("MeshComponent");
        reg.register_component<RenderableComponent>("RenderableComponent");
        reg.register_component<LightComponent>("LightComponent");
    }
}
