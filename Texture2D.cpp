
#include <GL/glew.h>

#include "Texture2D.hpp"

Texture2D::Texture2D() {
    glGenTextures(1, &handle);
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &handle);
}

GLuint Texture2D::getHandle() {
    return handle;
}

void Texture2D::bind() {
    glBindTexture(GL_TEXTURE_2D, handle);
}

void Texture2D::bind(int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    bind();
}

void Texture2D::create(Image image) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPointer());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // TODO seems to be needed if no mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture2D::setInterpolation(bool linear) {
    GLenum filter = linear ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

void Texture2D::setBorder(bool clamp) {
    GLenum wrap = clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
}

void Texture2D::setBorder(glm::vec4 const & color) {
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &color[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}
