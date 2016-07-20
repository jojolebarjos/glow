
#ifndef GLOW_TEXTURE2D_HPP
#define GLOW_TEXTURE2D_HPP

#include "Common.hpp"
#include "Image.hpp"

class Texture2D {
public:
    
    Texture2D();
    ~Texture2D();
    
    Texture2D(Texture2D const &) = delete;
    Texture2D & operator=(Texture2D const &) = delete;
    
    GLuint getHandle();
    
    void bind();
    void bind(int slot);
    
    void create(Image image);
    void create(int width, int height, bool floating = false);
    
    void setInterpolation(bool linear);
    
    void setBorder(bool clamp);
    void setBorder(glm::vec4 const & color);
    
    // TODO mipmaps?
    
    // TODO store size?
    
    // TODO download data?
    
private:
    
    GLuint handle;

};

#endif
