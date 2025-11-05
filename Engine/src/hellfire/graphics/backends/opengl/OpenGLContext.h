//
// Created by denzel on 25/09/2025.
//

#pragma once
#include "hellfire/graphics/renderer/graphics_api.h"

namespace hellfire {
    class OpenGLContext : public IGraphicsContext {
    public:
        void begin_frame() override;

        void end_frame() override;

        void clear(float r, float g, float a) override;

        void draw_mesh(uint32_t vertex_array, uint32_t index_count) override;

        void draw_instanced(uint32_t vertex_array, uint32_t index_count, uint32_t instance_count) override;

        uint32_t create_shader_program(const char *vertex_src, const char *fragment_src) override;

        void use_shader_program(uint32_t program_id) override;

        void set_uniform_mat4(uint32_t program_id, const char *name, const float *data) override;

        void set_uniform_vec3(uint32_t program_id, const char *name, float x, float y, float z) override;

        void set_uniform_float(uint32_t program_id, const char *name, float value) override;

        void set_uniform_int(uint32_t program_id, const char *name, int value) override;

        uint32_t create_framebuffer(uint32_t width, uint32_t height) override;

        void bind_framebuffer(uint32_t framebuffer_id) override;

        void unbind_framebuffer() override;

        uint32_t get_framebuffer_texture(uint32_t framebuffer_id) override;

        void resize_framebuffer(uint32_t framebuffer_id, uint32_t width, uint32_t height) override;

    private:
        RenderState current_state_;
    };
};
