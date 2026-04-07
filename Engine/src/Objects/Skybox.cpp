//
// Created by denzel on 14/06/2025.
//

#include "DCraft/Structs/Skybox.h"
#include <GL/glew.h>
#include <iostream>

#include <stb_image.h>

namespace hellfire {
    Skybox::~Skybox() {
        if (cubemap_texture_ != 0) glDeleteTextures(1, &cubemap_texture_);
    }

    void Skybox::set_cubemap_faces(const std::array<std::string, 6> &faces) {
        if (cubemap_texture_ != 0) {
            glDeleteTextures(1, &cubemap_texture_);
        }
        cubemap_texture_ = load_cubemap(faces);
    }

    uint32_t Skybox::load_cubemap(const std::array<std::string, 6> &faces) {
        uint32_t textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        // stbi_set_flip_vertically_on_load(true);
        int width, height, nrChannels;
        for (uint32_t i = 0; i < faces.size(); i++) {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            } else {
                std::cout << "cubemaps texture failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }

        // stbi_set_flip_vertically_on_load(false);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }
}
