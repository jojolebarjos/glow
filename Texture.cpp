
#include <GL/glew.h>

#include "Texture.hpp"

Texture::Texture() : width(0), height(0), mipmapped(false), depthStencil(false), multisampled(false) {
    glGenTextures(1, &handle);
}

Texture::~Texture() {
    glDeleteTextures(1, &handle);
}

GLuint Texture::getHandle() {
    return handle;
}

void Texture::createColor(Image const & image, bool mipmapped) {
    width = image.getWidth();
    height = image.getHeight();
    this->mipmapped = mipmapped;
    depthStencil = false;
    multisampled = false;
    glBindTexture(GL_TEXTURE_2D, handle);
    // TODO use glTexStorage instead https://www.opengl.org/wiki/Common_Mistakes#Creating_a_complete_texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPointer());
    if (mipmapped) {
        glEnable(GL_TEXTURE_2D);
        glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture::createColor(int width, int height, bool floating, bool multisampled) {
    this->width = width;
    this->height = height;
    mipmapped = false;
    depthStencil = false;
    this->multisampled = multisampled;
    if (multisampled) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, handle);
        // TODO give this 4 as a parameter (1, 2, 4, 16, more is useless)
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, floating ? GL_RGBA16F : GL_RGBA8, width, height, GL_TRUE);
    } else {
        glBindTexture(GL_TEXTURE_2D, handle);
        if (floating)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
}

void Texture::createDepthStencil(GLuint width, GLuint height, bool multisampled) {
    this->width = width;
    this->height = height;
    mipmapped = false;
    depthStencil = true;
    this->multisampled = multisampled;
    if (multisampled) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, handle);
        // TODO give this 4 as a parameter (1, 2, 4, 16, more is useless)
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH24_STENCIL8, width, height, GL_TRUE);
    } else {
        glBindTexture(GL_TEXTURE_2D, handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    }
}

GLuint Texture::getWidth() const {
    return width;
}

GLuint Texture::getHeight() const {
    return height;
}

bool Texture::isMipmapped() const {
    return mipmapped;
}

bool Texture::isDepthStencil() const {
    return depthStencil;
}

bool Texture::isMultisampled() const {
    return multisampled;
}

void Texture::bind() {
    glBindTexture(multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, handle);
}

void Texture::bind(int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    bind();
}

void Texture::setInterpolation(bool linear) {
    if (depthStencil || multisampled) {
        assert(false);
        return;
    }
    if (mipmapped)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
    else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
}

void Texture::setAnisotropy(bool enabled) {
    if (depthStencil || multisampled) {
        assert(false);
        return;
    }
    if (enabled) {
        GLfloat max;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max);
    } else
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0.0f);
}

void Texture::setBorder(bool clamp) {
    if (depthStencil || multisampled) {
        assert(false);
        return;
    }
    GLenum wrap = clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
}

void Texture::setBorder(glm::vec4 const & color) {
    if (depthStencil || multisampled) {
        assert(false);
        return;
    }
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &color[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}
