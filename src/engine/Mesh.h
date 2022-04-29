#pragma once

#include <GL/glew.h>
#include <memory>
#include <vector>

class Mesh;

using MeshPtr = std::shared_ptr<Mesh>;

class Mesh {
public:
    Mesh(const std::vector<float> &points, const std::vector<unsigned int> &indices, GLenum usage = GL_STATIC_DRAW);

    void setAttribute(GLuint index, GLint size, GLenum type, GLboolean normalize,
                      GLsizei stride, std::size_t offset) const;

    void draw() const;

protected:
    GLuint vao, vbo, ibo;

    std::size_t index_count;
};

