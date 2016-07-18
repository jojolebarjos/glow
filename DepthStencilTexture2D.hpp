
#ifndef GLOW_DEPTHSTENCILTEXTURE2D_HPP
#define GLOW_DEPTHSTENCILTEXTURE2D_HPP

#include "Common.hpp"

class DepthStencilTexture2D {
public:
    
    // TODO refactor textures to avoid code duplication and have unified semantic in framebuffer?
    
    DepthStencilTexture2D();
    ~DepthStencilTexture2D();
    
    DepthStencilTexture2D(DepthStencilTexture2D const &) = delete;
    DepthStencilTexture2D & operator=(DepthStencilTexture2D const &) = delete;
    
    GLuint getHandle();
    
    void bind();
    void bind(int slot);
    
    void create(GLuint width, GLuint height);
    
private:

    GLuint handle;
    
};

#endif
