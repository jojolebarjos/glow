
#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include "Texture2D.hpp"
#include "DepthStencilTexture2D.hpp"

class Framebuffer {
public:
    
    Framebuffer();
    ~Framebuffer();
    
    Framebuffer(Framebuffer const &) = delete;
    Framebuffer & operator=(Framebuffer const &) = delete;
    
    void bind();
    
    void attach(Texture2D & texture, int attachment);
    void attach(DepthStencilTexture2D & texture);
    
    bool validate();
    
    // TODO get textures, get size...
    
private:
    
    GLuint handle;

};

#endif
