
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

void VertexArray::addAttribute(int index, GLint size, GLenum type, GLuint stride, GLuint offset, bool instanced) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void *)(intptr_t)offset);
    glVertexAttribDivisor(index, instanced ? 1 : 0);
}

void VertexArray::addAttributeMat4(int index, GLuint stride, GLuint offset, bool instanced) {
    if (stride == 0)
        stride = 64;
    addAttribute(index + 0, 4, GL_FLOAT, stride, offset +  0, instanced);
    addAttribute(index + 1, 4, GL_FLOAT, stride, offset + 16, instanced);
    addAttribute(index + 2, 4, GL_FLOAT, stride, offset + 32, instanced);
    addAttribute(index + 3, 4, GL_FLOAT, stride, offset + 48, instanced);
}
