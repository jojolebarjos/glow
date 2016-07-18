
#include "Framebuffer.hpp"

Framebuffer::Framebuffer() {
    glGenFramebuffers(1, &handle);
    assert(handle);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &handle);
}

void Framebuffer::attach(Texture2D & texture) {
    // TODO
}

bool validate() {
    // TODO
    return false;
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, handle);
}
