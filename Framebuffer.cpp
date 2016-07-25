
#include "Framebuffer.hpp"

Framebuffer::Framebuffer() : color(0) {
    glGenFramebuffers(1, &handle);
    assert(handle);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &handle);
}

GLuint Framebuffer::getHandle() const {
    return handle;
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, handle);
}

void Framebuffer::attach(Texture & texture) {
    GLenum target = texture.getMultisampling() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    if (texture.isDepthStencil())
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, target, texture.getHandle(), 0);
    else {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + color, target, texture.getHandle(), 0);
        ++color;
    }
}

bool Framebuffer::validate() {
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}
