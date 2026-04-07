#include "DCraft/Graphics/Textures/Texture.h"

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace DCraft
{
    Texture::Texture(const std::string& path)
    {
        glGenTextures(1, &texture_id_);

        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);

        if (data)
        {

        }
    }

    void Texture::bind(unsigned int slot)
    {
    }

    void Texture::unbind()
    {
    }
}
