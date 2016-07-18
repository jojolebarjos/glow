
#include "Framebuffer.hpp"

Framebuffer::Framebuffer() {
    glGenFramebuffers(1, &handle);
    assert(handle);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &handle);
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, handle);
}

void Framebuffer::attach(Texture2D & texture, int attachment) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, GL_TEXTURE_2D, texture.getHandle(), 0);
}

void Framebuffer::attach(DepthStencilTexture2D & texture) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture.getHandle(), 0);
}

bool Framebuffer::validate() {
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}
