//
// Created by denzel on 11/08/2025.
//

#include <hellfire/ecs/ScriptComponent.h>
#include "hellfire/ecs/TransformComponent.h"

namespace hellfire {
    TransformComponent * ScriptComponent::get_transform() const {
        return get_owner().transform();
    }
}
