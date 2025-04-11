//
// Created by denzel on 07/04/2025.
//

#include "DCraft/Graphics/Lights/Light.h"

namespace DCraft {
    Light::Light(const std::string &name) : Object3D(name) {
    }

    json Light::to_json() {
        json j = Object3D::to_json();
        j["type"] = "Light";
        j["color"] = { color.r, color.g, color.b };
        j["intensity"] = intensity;
        return j;
    }
}
