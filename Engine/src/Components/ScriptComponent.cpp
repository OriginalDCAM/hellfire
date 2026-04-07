//
// Created by denzel on 11/08/2025.
//

#include <DCraft/Components/ScriptComponent.h>
#include "DCraft/Components/TransformComponent.h"
#include <DCraft/Structs/Entity.h>

namespace DCraft {
    TransformComponent * ScriptComponent::get_transform() const {
        return get_owner() ? get_owner()->get_component<TransformComponent>() : nullptr;
    }
}
