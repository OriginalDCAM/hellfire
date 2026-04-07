//
// Created by denzel on 17/08/2025.
//

#pragma once
#include <vector>

#include "hellfire/ecs/InstancedRenderableComponent.h"

namespace hellfire {
    class Material;
    class Entity;
}

class AsteroidBeltFactory {
public:
    static hellfire::Entity* create_asteroid_belt(size_t quantity);
private:

    
static std::vector<hellfire::InstancedRenderableComponent::InstanceData> generate_asteroid_belt_data(const size_t quantity);

    static std::shared_ptr<hellfire::Material> create_rocky_instanced_material();

    static std::shared_ptr<hellfire::Material> create_metallic_instanced_material();

    static std::shared_ptr<hellfire::Material> create_icy_instanced_material();

    static std::shared_ptr<hellfire::Material> create_crystal_instanced_material();

    static std::shared_ptr<hellfire::Material> create_dusty_instanced_material();
};
