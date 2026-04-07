#include "DCraft/Graphics/Textures/Texture.h"

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stb/stb_image.h>

namespace DCraft {
    Texture::Texture(const std::string &path, TextureType type) : path_(path), type_(type) {
        glGenTextures(1, &texture_id_);
        glBindTexture(GL_TEXTURE_2D, texture_id_);

        // Set unpack alignment to 1
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);

        if (data) {
            GLenum format;
            switch (nr_channels) {
                case 1:
                    format = GL_RED;
                break;
                case 3:
                    format = GL_RGB;
                break;
                case 4:
                    format = GL_RGBA;
                break;
                default:
                    format = GL_RGB;
                break;
            }
            
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            std::cerr << "Failed to load texture: " << path << std::endl;
        }

        stbi_set_flip_vertically_on_load(false);
    }

    void Texture::bind(unsigned int slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
    }

    void Texture::unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
