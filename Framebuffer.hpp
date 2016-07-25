
#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include "Texture.hpp"

class Framebuffer {
public:
    
    Framebuffer();
    ~Framebuffer();
    
    Framebuffer(Framebuffer const &) = delete;
    Framebuffer & operator=(Framebuffer const &) = delete;
    
    // TODO can store default framebuffer too?
    
    GLuint getHandle() const;
    
    void bind();
    
    void attach(Texture & texture);
    
    bool validate();
    
    // TODO get textures, get size...
    
private:
    
    GLuint handle;
    int color;

};

#endif
