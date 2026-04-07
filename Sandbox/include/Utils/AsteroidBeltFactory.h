//
// Created by denzel on 17/08/2025.
//

#pragma once
#include <vector>

#include "DCraft/Components/InstancedRenderableComponent.h"

namespace DCraft {
    class Material;
    class Entity;
}

class AsteroidBeltFactory {
public:
    static DCraft::Entity* create_asteroid_belt(size_t quantity);
private:

    
static std::vector<DCraft::InstancedRenderableComponent::InstanceData> generate_asteroid_belt_data(const size_t quantity);

    static DCraft::Material* create_rocky_instanced_material();

    static DCraft::Material* create_metallic_instanced_material();

    static DCraft::Material* create_icy_instanced_material();

    static DCraft::Material* create_crystal_instanced_material();

    static DCraft::Material* create_dusty_instanced_material();
};
