//
// Created by denzel on 31/05/2025.
//
#include "Framebuffer.h"

#include <iostream>
#include <GL/glew.h>

namespace hellfire {
    Framebuffer::Framebuffer() : framebuffer_id_(0), depth_attachment_(0), stencil_attachment_(0) {
        create_framebuffer();
    }

    Framebuffer::~Framebuffer() {
        cleanup();
    }

    void Framebuffer::attach_color_texture(const FrameBufferAttachmentSettings &settings) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);

        uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Allocate texture storage
        glTexImage2D(GL_TEXTURE_2D, 0, settings.internal_format,
                     settings.width, settings.height, 0,
                     settings.format, settings.type, nullptr);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings.min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings.mag_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings.wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings.wrap_t);

        // Attach to framebuffer
        const uint32_t attachment_point = GL_COLOR_ATTACHMENT0 + color_attachments_.size();
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_point, GL_TEXTURE_2D, texture, 0);

        color_attachments_.push_back(texture);
        color_settings_.push_back(settings);

        // Update draw buffers
        std::vector<GLenum> draw_buffers;
        for (size_t i = 0; i < color_attachments_.size(); i++) {
            draw_buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
        glDrawBuffers(draw_buffers.size(), draw_buffers.data());

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::attach_texture_by_id(const uint32_t texture_id, GLenum attachment = GL_COLOR_ATTACHMENT0, GLenum target = GL_FRAMEBUFFER) {
        // Attach to framebuffer
        glFramebufferTexture2D(target, attachment, 
                  GL_TEXTURE_2D, texture_id, 0);
    }

    void Framebuffer::attach_depth_texture(const FrameBufferAttachmentSettings &settings) {
        if (depth_attachment_ != 0) {
            std::cerr << "Depth attachment already exists!" << std::endl;
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);

        glGenTextures(1, &depth_attachment_);
        glBindTexture(GL_TEXTURE_2D, depth_attachment_);

        // Override format settings for depth texture
        const GLenum internal_format = settings.internal_format == GL_RGBA8
                                           ? GL_DEPTH_COMPONENT32F
                                           : settings.internal_format;
        constexpr GLenum format = GL_DEPTH_COMPONENT;
        const GLenum type = settings.type == GL_UNSIGNED_BYTE ? GL_FLOAT : settings.type;

        // Allocate texture storage
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format,
                     settings.width, settings.height, 0,
                     format, type, nullptr);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings.min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings.mag_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings.wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings.wrap_t);

        // Attach to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_attachment_, 0);

        // Unbind texture & framebuffer
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        depth_settings_ = settings; 
        has_depth_ = true;
    }

    void Framebuffer::attach_stencil_texture(const FrameBufferAttachmentSettings &settings) {
        if (stencil_attachment_ != 0) {
            std::cerr << "Stencil attachment already exists!" << std::endl;
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);

        glGenTextures(1, &stencil_attachment_);
        glBindTexture(GL_TEXTURE_2D, stencil_attachment_);

        // Override format settings for stencil texture
        constexpr GLenum internal_format = GL_STENCIL_INDEX8;
        constexpr GLenum format = GL_STENCIL_INDEX;
        constexpr GLenum type = GL_UNSIGNED_BYTE;

        glTexImage2D(GL_TEXTURE_2D, 0, internal_format,
                     settings.width, settings.height, 0,
                     format, type, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings.wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings.wrap_t);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencil_attachment_, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        stencil_settings_ = settings;  // Store the settings
        has_stencil_ = true;
    }


    void Framebuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);

        // Set viewport based on first color attachment if it exists
        if (!color_settings_.empty()) {
            glViewport(0, 0, color_settings_[0].width, color_settings_[0].height);
        }
    }

    void Framebuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::resize(const uint32_t width, const uint32_t height) {
        // Store current settings
        bool had_depth = depth_attachment_ != 0;
        bool had_stencil = stencil_attachment_ != 0;
        
        std::vector<FrameBufferAttachmentSettings> old_color_settings = color_settings_;
        FrameBufferAttachmentSettings old_depth_settings = depth_settings_;
        FrameBufferAttachmentSettings old_stencil_settings = stencil_settings_;

        // Cleanup old resources
        cleanup();

        // Recreate framebuffer
        create_framebuffer();

        // Recreate attachments with new size
        for (auto &settings: old_color_settings) {
            settings.width = width;
            settings.height = height;
            attach_color_texture(settings);
        }

        if (had_depth) {
            old_depth_settings.width = width;
            old_depth_settings.height = height;
            attach_depth_texture(old_depth_settings);
        }

        if (had_stencil) {
            old_stencil_settings.width = width;
            old_stencil_settings.height = height;
            attach_stencil_texture(old_stencil_settings);
        }

        if (!is_complete()) {
            std::cerr << "Framebuffer incomplete after resize!" << std::endl;
        }
    }

    uint32_t Framebuffer::read_pixel_from_texture(const uint32_t texture_id, const int x, const int y) {
        std::cout << "FBO ID: " << framebuffer_id_<< ", is valid: " << (glIsFramebuffer(framebuffer_id_) ? "yes" : "no") << std::endl;
        std::cout << "Texture ID: " << texture_id << ", is valid: " << (glIsTexture(texture_id) ? "yes" : "no") << std::endl;
    
        // Bind the framebuffer
        bind();
        // Attach the texture to the framebuffer
        attach_texture_by_id(texture_id, GL_COLOR_ATTACHMENT0, GL_READ_FRAMEBUFFER);

        // Set the read buffer to where the texture is attached
        glReadBuffer(GL_COLOR_ATTACHMENT0);

        // Read the pixel data at x, y position
        uint32_t pixel_data = 0;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixel_data);

        // Unbind the fbo
        unbind();
        return pixel_data;
    }
    

    bool Framebuffer::is_complete() const {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);
        const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer not complete! Status: ";
            switch (status) {
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                    std::cerr << "INCOMPLETE_ATTACHMENT" << std::endl;
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                    std::cerr << "INCOMPLETE_MISSING_ATTACHMENT" << std::endl;
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                    std::cerr << "INCOMPLETE_DRAW_BUFFER" << std::endl;
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                    std::cerr << "INCOMPLETE_READ_BUFFER" << std::endl;
                    break;
                case GL_FRAMEBUFFER_UNSUPPORTED:
                    std::cerr << "UNSUPPORTED" << std::endl;
                    break;
                default:
                    std::cerr << "UNKNOWN (" << status << ")" << std::endl;
                    break;
            }
            return false;
        }

        return true;
    }

    uint32_t Framebuffer::get_width() const {
        return !color_settings_.empty() ? color_settings_[0].width : 0;
    }

    uint32_t Framebuffer::get_height() const {
        return !color_settings_.empty() ? color_settings_[0].height : 0;
    }

    void Framebuffer::create_framebuffer() {
        glGenFramebuffers(1, &framebuffer_id_);
    }

    void Framebuffer::cleanup() {
        if (!color_attachments_.empty()) {
            glDeleteTextures(static_cast<GLsizei>(color_attachments_.size()), color_attachments_.data());
            color_attachments_.clear();
            color_settings_.clear();
        }
        if (depth_attachment_) {
            glDeleteTextures(1, &depth_attachment_);
            depth_attachment_ = 0;
        }
        if (stencil_attachment_) {
            glDeleteTextures(1, &stencil_attachment_);
            stencil_attachment_ = 0;
        }
        if (framebuffer_id_) {
            glDeleteFramebuffers(1, &framebuffer_id_);
            framebuffer_id_ = 0;
        }
    }
}
