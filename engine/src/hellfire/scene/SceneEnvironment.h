//
// Created by denzel on 14/11/2025.
//

#pragma once
#include <memory>

#include "hellfire/graphics/Skybox.h"

namespace hellfire {
    class SceneEnvironment {
    public:
        SceneEnvironment() : skybox_(nullptr) {}
        void set_ambient_light(const float intensity) {
            ambient_light_ = glm::vec3(intensity);
        }

        glm::vec3 get_ambient_light() const { return ambient_light_; }

        // Skybox management
        void set_skybox(Skybox &skybox) {
            skybox_.reset(&skybox);
        }

        Skybox* get_skybox() const { return skybox_.get(); }
        bool has_skybox() const { return skybox_ != nullptr; }
    private:
        glm::vec3 ambient_light_ = glm::vec3(0.4f);
        std::unique_ptr<Skybox> skybox_;
    };
}
