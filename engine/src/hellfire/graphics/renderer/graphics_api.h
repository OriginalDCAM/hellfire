//
// Created by denzel on 25/09/2025.
//

#pragma once
#include <cstdint>

namespace hellfire {
    enum class BlendMode {
        NONE,
        ALPHA,
        ADDITIVE,
        MULTIPLY
    };

    enum class DepthTest {
        DISABLED,
        LESS,
        LESS_EQUAL,
        GREATER,
        EQUAL
    };

    enum class CullMode {
        NONE,
        FRONT,
        BACK,
        FRONT_AND_BACK
    };

    struct RenderState {
        BlendMode blend_mode = BlendMode::NONE;
        DepthTest depth_test = DepthTest::LESS;
        CullMode cull_mode = CullMode::BACK;
        bool depth_write = true;
        bool wireframe = false;
    };

    struct ShadowSettings {
        
    };

    /// Abstract graphics context
    class IGraphicsContext {
    public:
        virtual ~IGraphicsContext() = default;

        // Frame management
        virtual void begin_frame() = 0;
        virtual void end_frame() = 0;
        virtual void clear(float r, float g, float b) = 0;
        
        // Drawing
        virtual void draw_mesh(uint32_t vertex_array, uint32_t index_count) = 0;
        virtual void draw_instanced(uint32_t vertex_array, uint32_t index_count, uint32_t instance_count) = 0;

        // Resource management
        virtual uint32_t create_shader_program(const char* vertex_src, const char* fragment_src) = 0;
        virtual void use_shader_program(uint32_t program_id) = 0;
        virtual void set_uniform_mat4(uint32_t program_id, const char* name, const float* data) = 0;
        virtual void set_uniform_vec3(uint32_t program_id, const char* name, float x, float y, float z) = 0;
        virtual void set_uniform_float(uint32_t program_id, const char* name, float value) = 0;
        virtual void set_uniform_int(uint32_t program_id, const char* name, int value) = 0;

        // Framebuffer operations
        virtual uint32_t create_framebuffer(uint32_t width, uint32_t height) = 0;
        virtual void bind_framebuffer(uint32_t framebuffer_id) = 0;
        virtual void unbind_framebuffer() = 0;
        virtual uint32_t get_framebuffer_texture(uint32_t framebuffer_id) = 0;
        virtual void resize_framebuffer(uint32_t framebuffer_id, uint32_t width, uint32_t height) = 0;
    };
}
