
#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include "Texture2D.hpp"

class Framebuffer {
public:
    
    Framebuffer();
    ~Framebuffer();
    
    Framebuffer(Framebuffer const &) = delete;
    Framebuffer & operator=(Framebuffer const &) = delete;
    
    void attach(Texture2D & texture);
    // TODO stencil/depth...
    bool validate();
    
    void bind();
    
    // TODO get textures, get size...
    
private:
    
    GLuint handle;

};

#endif
