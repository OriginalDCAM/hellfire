#pragma once
#include <cstdint>
#include <string>

namespace DCraft
{
    class Texture
    {
    public:
        int width, height;
        /// 
        /// Number of color channels
        /// 
        int nr_channels;

        [[nodiscard]]
        Texture(const std::string& path);
        void bind(unsigned int slot = 0);
        void unbind();

    private:
        uint32_t texture_id_;
    };
}

