
#ifndef GLOW_IMAGE_HPP
#define GLOW_IMAGE_HPP

#include "Common.hpp"

class Image {
public:
    
    Image();
    
    GLuint getWidth() const;
    GLuint getHeight() const;
    
    GLuint const * getPointer() const;
    
    bool load(std::string const & path);
    bool loadBmp(std::string const & path);
    bool loadPng(std::string const & path);
    bool loadJpg(std::string const & path);
    
    // TODO save?
    
    // TODO resize? raw access to bytes?
    
private:

    GLuint width;
    GLuint height;
    std::vector<GLuint> colors;
    
};

#endif
