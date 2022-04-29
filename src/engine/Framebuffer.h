#pragma once


#include <memory>
#include <GL/glew.h>
#include <logger/logger.h>
#include <cstdlib>
#include "Texture.h"
;

class Framebuffer;

using FramebufferPtr = std::shared_ptr<Framebuffer>;

class Framebuffer {
public:
    Framebuffer(size_t width, size_t height)
            : width(width), height(height)
    {
        glGenFramebuffers(1, &this->id);
        this->bind();

        render_texture = std::make_shared<Texture>(GL_TEXTURE_2D);
        render_texture->setTexImage2D(0, GL_RGB16F, this->width, this->height, GL_RGB, GL_FLOAT, nullptr);
        render_texture->setFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0);
//        render_texture->setTexImage2D(0, GL_DEPTH_COMPONENT24, this->width, this->height, GL_DEPTH_COMPONENT,
//                                      GL_FLOAT, nullptr);
//        render_texture->setFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0);

        depthRenderbuffer = std::make_shared<Renderbuffer>();
        depthRenderbuffer->setRenderbufferStorage(GL_DEPTH_COMPONENT, this->width, this->height);
        depthRenderbuffer->setFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT);

        GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, buffers);
//        glDrawBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            LOGE("Failed to setup framebuffer", nullptr);
        }

    }

    void bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, this->id);
    }

    void unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void begin_render() {
        this->bind();
        glViewport(0, 0, this->width, this->height);
        glClearColor(1, 1, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    TexturePtr get_render_texture() {
        return this->render_texture;
    }

private:
    size_t width, height;
    GLuint id;
    TexturePtr render_texture = nullptr;
    RenderbufferPtr depthRenderbuffer = nullptr;

};

