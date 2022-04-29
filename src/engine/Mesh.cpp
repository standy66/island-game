
#include "Mesh.h"
#include <logger/logger.h>

Mesh::Mesh(const std::vector<float> &points, const std::vector<unsigned int> &indices, GLenum usage) {
    glGenBuffers(1, &this->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &this->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), usage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    this->index_count = indices.size();

    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::setAttribute(GLuint index, GLint size, GLenum type, GLboolean normalize,
                        GLsizei stride, std::size_t offset) const {
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, normalize, stride, reinterpret_cast<void*>(offset));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void Mesh::draw() const {
    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, this->index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
