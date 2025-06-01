//
// Created by denzel on 31/05/2025.
//

#include <cstdint>

namespace DCraft {
    class Framebuffer {
    public:
        Framebuffer(uint32_t width, uint32_t height, bool with_depth = true);
        ~Framebuffer();

        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;

        void bind();
        void unbind();
        void resize(uint32_t width, uint32_t height);

        uint32_t get_color_texture() const { return color_texture_; }
        uint32_t get_depth_texture() const { return depth_texture_; }
        uint32_t get_width() const { return width_; }
        uint32_t get_height() const { return height_; }

        bool is_complete()  const;

    private:
        void create_framebuffer();
        void cleanup();

        uint32_t framebuffer_id_;
        uint32_t color_texture_;
        uint32_t depth_texture_;
        uint32_t width_;
        uint32_t height_;
        bool with_depth_;
 
    };
    
}
