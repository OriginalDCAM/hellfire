//
// Created by denzel on 31/05/2025.
//
#include  "DCraft/Graphics/OGL/Framebuffer.h"

#include <iostream>
#include <GL/glew.h>

namespace DCraft {
    Framebuffer::Framebuffer(uint32_t width, uint32_t height, bool with_depth) : width_(width), height_(height),
        with_depth_(with_depth), framebuffer_id_(0), color_texture_(0), depth_texture_(0) {
        create_framebuffer();
    }

    Framebuffer::~Framebuffer() {
        cleanup();
    }

    void Framebuffer::create_framebuffer() {
        // Generate framebuffer
        glGenFramebuffers(1, &framebuffer_id_);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);

        // Create color texture
        glGenTextures(1, &color_texture_);
        glBindTexture(GL_TEXTURE_2D, color_texture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_, 0);

        if (with_depth_) {
            glGenTextures(1, &depth_texture_);
            glBindTexture(GL_TEXTURE_2D, depth_texture_);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                         nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_, 0);
        }

        if (!is_complete()) {
            std::cerr << "Framebuffer not complete! Status: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;

            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            switch (status) {
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                    std::cerr << "Framebuffer incomplete: Attachment is NOT complete." << std::endl;
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                    std::cerr << "Framebuffer incomplete: No image is attached to FBO." << std::endl;
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                    std::cerr << "Framebuffer incomplete: Draw buffer." << std::endl;
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                    std::cerr << "Framebuffer incomplete: Read buffer." << std::endl;
                    break;
                case GL_FRAMEBUFFER_UNSUPPORTED:
                    std::cerr << "Framebuffer incomplete: Unsupported by FBO implementation." << std::endl;
                    break;
                default:
                    std::cerr << "Framebuffer incomplete: Unknown error." << std::endl;
                    break;
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::cleanup() {
        if (color_texture_) {
            glDeleteTextures(1, &color_texture_);
            color_texture_ = 0;
        }
        if (depth_texture_) {
            glDeleteTextures(1, &depth_texture_);
            depth_texture_ = 0;
        }
        if (framebuffer_id_) {
            glDeleteFramebuffers(1, &framebuffer_id_);
            framebuffer_id_ = 0;
        }
    }

    void Framebuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);
        glViewport(0, 0, width_, height_);
    }

    void Framebuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::resize(uint32_t width, uint32_t height) {
        if (width_ == width && height_ == height) {
            return;
        }

        width_ = width;
        height_ = height;

        cleanup();
        create_framebuffer();
    }

    bool Framebuffer::is_complete() const {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return status == GL_FRAMEBUFFER_COMPLETE;
    }
}
