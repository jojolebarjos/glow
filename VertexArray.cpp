
#include "VertexArray.hpp"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &handle);
    assert(handle);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &handle);
}

void VertexArray::bind() {
    glBindVertexArray(handle);
}

void VertexArray::addAttribute(int index, GLint size, GLenum type, GLuint stride, GLuint offset) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void *)(intptr_t)offset);
}
