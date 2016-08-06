
#include "Buffer.hpp"

Buffer::Buffer() {
    glGenBuffers(1, &handle);
    assert(handle);
    target = GL_NONE;
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &handle);
}

GLuint Buffer::getHandle() const {
    return handle;
}

void Buffer::bind(GLenum target) {
    glBindBuffer(target, handle);
    this->target = target;
}

void Buffer::setData(GLuint size, void const * pointer, GLenum usage) {
    glBufferData(target, size, pointer, usage);
}

void Buffer::setSubData(GLuint offset, GLuint size, void const * pointer) {
    glBufferSubData(target, offset, size, pointer);
}

void Buffer::getSubData(GLuint offset, GLuint size, void * pointer) {
    glGetBufferSubData(target, offset, size, pointer);
}

void * Buffer::map(GLbitfield access) {
    return glMapBuffer(target, access);
}

void * Buffer::map(GLintptr offset, GLsizeiptr size, GLbitfield access) {
    return glMapBufferRange(target, offset, size, access);
}

void Buffer::unmap() {
    glUnmapBuffer(target);
}
