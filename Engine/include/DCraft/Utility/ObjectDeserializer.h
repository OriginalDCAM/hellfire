//
// Created by denzel on 11/04/2025.
//
#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "MaterialSerializer.h"
#include "DCraft/Addons/ModelLoader.h"

using json = nlohmann::json;
using namespace DCraft;

namespace DCraft {
    class Light;
    class DirectionalLight;
    class Cameras;
    class SceneManager;

}
namespace DCraft {
    class ObjectDeserializer {

    };
}