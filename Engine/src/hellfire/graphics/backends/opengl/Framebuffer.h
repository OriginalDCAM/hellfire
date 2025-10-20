//
// Created by denzel on 31/05/2025.
//
#pragma once

#include <cstdint>
#include <vector>

#include "GL/glew.h"

namespace hellfire {
    struct FrameBufferAttachmentSettings {
        uint32_t width = 512;
        uint32_t height = 512;
        // Texture format
        GLenum internal_format = GL_RGBA8;
        GLenum format = GL_RGBA;
        GLenum type = GL_UNSIGNED_BYTE;

        // Filtering
        GLenum min_filter = GL_LINEAR;
        GLenum mag_filter = GL_LINEAR;

        // Wrapping
        GLenum wrap_s = GL_CLAMP_TO_EDGE;
        GLenum wrap_t = GL_CLAMP_TO_EDGE;
    };

    class Framebuffer {
    public:
        Framebuffer();

        ~Framebuffer();

        void attach_color_texture(const FrameBufferAttachmentSettings &settings = {});

        void attach_depth_texture(const FrameBufferAttachmentSettings &settings = {});

        void attach_stencil_texture(const FrameBufferAttachmentSettings &settings = {});


        Framebuffer(const Framebuffer &) = delete;

        Framebuffer &operator=(const Framebuffer &) = delete;

        void bind() const;

        void unbind();

        void resize(uint32_t width, uint32_t height);

        const std::vector<uint32_t> &get_color_attachments() const { return color_attachments_; }

        uint32_t get_color_attachment(const size_t index = 0) const {
            return index < color_attachments_.size() ? color_attachments_[index] : 0;
        }

        size_t get_color_attachment_count() const { return color_attachments_.size(); }
        uint32_t get_depth_attachment() const { return depth_attachment_; }

        bool is_complete() const;

        uint32_t get_width() const;

        uint32_t get_height() const;

    private:
        void create_framebuffer();

        void cleanup();

        uint32_t framebuffer_id_;
        std::vector<uint32_t> color_attachments_;
        uint32_t depth_attachment_;
        uint32_t stencil_attachment_;
        std::vector<FrameBufferAttachmentSettings> color_settings_;
        FrameBufferAttachmentSettings depth_settings_;
        FrameBufferAttachmentSettings stencil_settings_;
        bool has_depth_ = false;
        bool has_stencil_ = false;
    };
}
