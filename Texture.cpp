
#include <GL/glew.h>

#include "Texture.hpp"

Texture::Texture() : width(0), height(0), depth(0), mipmapped(false), depthStencil(false), multisampling(0) {
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
    depth = 0;
    this->mipmapped = mipmapped;
    depthStencil = false;
    multisampling = 0;
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPointer());
    if (mipmapped) {
        //glEnable(GL_TEXTURE_2D);
        glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture::createColor(uint32_t width, uint32_t height, bool floating, GLuint multisampling) {
    this->width = width;
    this->height = height;
    depth = 0;
    mipmapped = false;
    depthStencil = false;
    this->multisampling = multisampling;
    if (multisampling) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, handle);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisampling, floating ? GL_RGBA16F : GL_RGBA8, width, height, GL_TRUE);
    } else {
        glBindTexture(GL_TEXTURE_2D, handle);
        if (floating)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
}

void Texture::createDepthStencil(uint32_t width, uint32_t height, GLuint multisampling) {
    this->width = width;
    this->height = height;
    depth = 0;
    mipmapped = false;
    depthStencil = true;
    this->multisampling = multisampling;
    if (multisampling) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, handle);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisampling, GL_DEPTH24_STENCIL8, width, height, GL_TRUE);
    } else {
        glBindTexture(GL_TEXTURE_2D, handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    }
}

void Texture::createColorArray(std::vector<Image const *> images, bool mipmapped) {
    if (images.empty()) {
        assert(false);
        return;
    }
    width = images[0]->getWidth();
    height = images[0]->getHeight();
    depth = images.size();
    this->mipmapped = mipmapped;
    depthStencil = false;
    multisampling = 0;
    glBindTexture(GL_TEXTURE_2D_ARRAY, handle);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    for (uint32_t i = 0; i < depth; ++i)
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, images[i]->getPointer());
    if (mipmapped) {
        glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

uint32_t Texture::getWidth() const {
    return width;
}

uint32_t Texture::getHeight() const {
    return height;
}

uint32_t Texture::getDepth() const {
    return depth;
}

bool Texture::isArray() const {
    return depth;
}

bool Texture::isMipmapped() const {
    return mipmapped;
}

bool Texture::isDepthStencil() const {
    return depthStencil;
}

GLuint Texture::getMultisampling() const {
    return multisampling;
}

void Texture::bind() {
    glBindTexture(multisampling ? GL_TEXTURE_2D_MULTISAMPLE : depth ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, handle);
}

void Texture::bind(int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    bind();
}

void Texture::setInterpolation(bool linear) {
    if (depthStencil || multisampling) {
        assert(false);
        return;
    }
    GLenum target = depth ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
    if (mipmapped)
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
    else
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
}

void Texture::setAnisotropy(bool enabled) {
    if (depthStencil || multisampling) {
        assert(false);
        return;
    }
    GLenum target = depth ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
    if (enabled) {
        GLfloat max;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max);
        glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, max);
    } else
        glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0.0f);
}

void Texture::setBorder(bool clamp) {
    if (depthStencil || multisampling) {
        assert(false);
        return;
    }
    GLenum target = depth ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
    GLenum wrap = clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
}

void Texture::setBorder(glm::vec4 const & color) {
    if (depthStencil || multisampling) {
        assert(false);
        return;
    }
    GLenum target = depth ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
    glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, &color[0]);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}
