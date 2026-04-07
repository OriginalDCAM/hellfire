//
// Created by denzel on 07/04/2025.
//

#include "DCraft.h"
#include "DCraft/Graphics/OGL/StandardMaterial.h"

#ifndef SANDBOXSCENE_H
#define SANDBOXSCENE_H

#include "DCraft.h"
#include <unordered_map>
#include <string>

// Forward declarations
namespace DCraft {
    class Material;
    class Scene;
    class SceneManager;
    struct WindowInfo;
}

// Typedef for readability
typedef std::unordered_map<std::string, DCraft::Material *> MaterialMap;

// Function declarations only
MaterialMap load_material_map();
DCraft::Scene* load_scene(DCraft::SceneManager& scene_manager, DCraft::WindowInfo window);

#endif // SANDBOXSCENE_H


