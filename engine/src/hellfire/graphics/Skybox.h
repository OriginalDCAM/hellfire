//
// Created by denzel on 14/06/2025.
//

#pragma once
#include <array>
#include <string>
#include <glm/glm.hpp>

namespace hellfire {
    class Skybox {
    public:
        Skybox() = default;

        ~Skybox();

        void set_cubemap_faces(const std::array<std::string, 6> &faces);

        void set_tint(const glm::vec3 &tint) {
            tint_ = tint;
        }

        void set_exposure(float exposure) {
            exposure_ = exposure;
        }

        uint32_t get_cubemap() const { return cubemap_texture_; }
        glm::vec3 get_tint() const { return tint_; }
        float get_exposure() const { return exposure_; }

        bool is_loaded() const { return cubemap_texture_ != 0; }
    private:
        uint32_t load_cubemap(const std::array<std::string, 6>& faces);

        // TODO: store the path's so the editor can use them
        
        uint32_t cubemap_texture_ = 0;
        glm::vec3 tint_ = glm::vec3(1.0f);
        float exposure_ = 1.0f;
    };
};
