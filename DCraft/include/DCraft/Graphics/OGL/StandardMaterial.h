//
// Created by denzel on 07/04/2025.
//

#pragma once
#include "DCraft/Graphics/Materials/Material.h"

namespace DCraft::OGL {
class StandardMaterial : public Material {
public:
    StandardMaterial(const std::string& name);

    void bind(void* renderer_context) override;
    
};
}


