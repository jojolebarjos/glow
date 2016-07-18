
#include "DepthStencilTexture2D.hpp"

DepthStencilTexture2D::DepthStencilTexture2D() {
    glGenTextures(1, &handle);
}

DepthStencilTexture2D::~DepthStencilTexture2D() {
    glDeleteTextures(1, &handle);
}

GLuint DepthStencilTexture2D::getHandle() {
    return handle;
}

void DepthStencilTexture2D::bind() {
    glBindTexture(GL_TEXTURE_2D, handle);
}

void DepthStencilTexture2D::bind(int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    bind();
}

void DepthStencilTexture2D::create(GLuint width, GLuint height) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0,  GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
}
