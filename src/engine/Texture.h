#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "logger/logger.h"

#include <memory>
#include <cstdlib>

class Texture;

using TexturePtr = std::shared_ptr<Texture>;

class Texture {
public:
    Texture(GLenum target = GL_TEXTURE_2D)
            :target(target) {
        glGenTextures(1, &this->texture);
    }

    ~Texture() {
        LOGI("Deleting texture", nullptr);
        glDeleteTextures(1, &this->texture);
    }

    Texture(const Texture&) = delete;

    void bind() const
    {
        glBindTexture(target, texture);
    }

    void unbind() const
    {
        glBindTexture(target, 0);
    }

    void generateMipmaps()
    {
        bind();
        glGenerateMipmap(target);
        unbind();
    }

    void setTexImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* data)
    {
        setTexImage2D(target, level, internalFormat, width, height, format, type, data);
    }

    void setTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* data) {
        bind();
        glTexImage2D(target, level, internalFormat, width, height, 0, format, type, data);
        GLenum error = glGetError();
        if (error != 0) {
            LOGE("Got error in setTexImage2D: %d, %s", error, glGetString(error));
        }
        unbind();
    }

    void setFramebufferTexture2D(GLenum target, GLenum attachement, GLint level) {
        this->bind();
        glFramebufferTexture2D(target, attachement, this->target, this->texture, level);
        GLenum error = glGetError();
        if (error != 0) {
            LOGE("Got error in setFramebufferTexture2D: %d, %s", error, glGetString(error));
        }
        this->unbind();
    }

    void setFramebufferTexture(GLenum target, GLenum attachement, GLint level) {
        this->bind();
        glFramebufferTexture(target, attachement, this->texture, level);
        GLenum error = glGetError();
        if (error != 0) {
            LOGE("Got error in setFramebufferTexture2D: %d, %s", error, glGetString(error));
        }
        this->unbind();
    }

    static TexturePtr load_texture(const std::string& filename, bool srgb);

private:
    GLenum target;
    GLuint texture;
};


class CubeTexture: public Texture {
public:
    CubeTexture()
            : Texture(GL_TEXTURE_CUBE_MAP) {

    }

    static TexturePtr load_cube_texture(const std::string& right, const std::string& left,
                                        const std::string& top, const std::string& bottom,
                                        const std::string& back, const std::string& front,
                                        bool srgb);
};

class Renderbuffer;

using RenderbufferPtr = std::shared_ptr<Renderbuffer>;

class Renderbuffer {
public:
    Renderbuffer() {
        glGenRenderbuffers(1, &this->id);
    }

    ~Renderbuffer() {
        LOGI("Deleting renderbuffer", nullptr);
        glDeleteRenderbuffers(1, &this->id);
    }

    Renderbuffer(const Renderbuffer&) = delete;

    void bind() const
    {
        glBindRenderbuffer(GL_RENDERBUFFER, this->id);
    }

    void unbind() const
    {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    void setRenderbufferStorage(GLenum internalFormat, GLsizei width, GLsizei height) {
        this->bind();
        glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
        GLenum error = glGetError();
        if (error != 0) {
            LOGE("Got error in setRenderbufferStorage: %d, %s", error, glGetString(error));
        }
        this->unbind();
    }

    void setFramebufferRenderbuffer(GLenum target, GLenum attachement) {
        GLenum error = glGetError();
        if (error != 0) {
            LOGE("Got error in setFramebufferRenderbuffer[before]: %d, %s", error, glGetString(error));
        }

        this->bind();
        glFramebufferRenderbuffer(target, attachement, GL_RENDERBUFFER, this->id);
        error = glGetError();
        if (error != 0) {
            LOGE("Got error in setFramebufferRenderbuffer: %d, %s", error, glGetString(error));
        }
        this->unbind();
    }



private:
    GLuint id;
};

class Sampler;

using SamplerPtr = std::shared_ptr<Sampler>;

class Sampler {
public:
    Sampler(GLint filter_mag = GL_NEAREST, GLint filter_min = GL_NEAREST,
            GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT, GLint wrap_r = GL_REPEAT) {
        glGenSamplers(1, &this->id);
        glSamplerParameteri(this->id, GL_TEXTURE_MAG_FILTER, filter_mag);
        glSamplerParameteri(this->id, GL_TEXTURE_MIN_FILTER, filter_min);
        glSamplerParameteri(this->id, GL_TEXTURE_WRAP_S, wrap_s);
        glSamplerParameteri(this->id, GL_TEXTURE_WRAP_T, wrap_t);
        glSamplerParameteri(this->id, GL_TEXTURE_WRAP_R, wrap_r);
    }

    void bind(int slot, TexturePtr texture) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindSampler(slot, this->id);
        texture->bind();
    }

private:
    GLuint id;
};