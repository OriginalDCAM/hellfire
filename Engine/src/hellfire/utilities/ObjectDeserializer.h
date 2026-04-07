//
// Created by denzel on 11/04/2025.
//
#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "MaterialSerializer.h"
#include "hellfire/assets/ModelLoader.h"

using json = nlohmann::json;
using namespace hellfire;

namespace hellfire {
    class Light;
    class DirectionalLight;
    class Cameras;
    class SceneManager;

}
namespace hellfire {
    class ObjectDeserializer {

    };
}