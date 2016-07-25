
#ifndef GLOW_TEXTURE2D_HPP
#define GLOW_TEXTURE2D_HPP

#include "Common.hpp"
#include "Image.hpp"

class Texture {
public:
    
    Texture();
    ~Texture();
    
    Texture(Texture const &) = delete;
    Texture & operator=(Texture const &) = delete;
    
    GLuint getHandle();
    
    void createColor(Image const & image, bool mipmapped = false);
    void createColor(int width, int height, bool floating = false, bool multisampled = false);
    
    void createDepthStencil(GLuint width, GLuint height, bool multisampled = false);
    
    GLuint getWidth() const;
    GLuint getHeight() const;
    
    bool isMipmapped() const;
    bool isDepthStencil() const;
    bool isMultisampled() const;
    
    void bind();
    void bind(int slot);
    
    void setInterpolation(bool linear);
    
    void setAnisotropy(bool enabled);
    
    void setBorder(bool clamp);
    void setBorder(glm::vec4 const & color);
    
    // TODO download data?
    
private:
    
    GLuint handle;
    GLuint width;
    GLuint height;
    bool mipmapped;
    bool depthStencil;
    bool multisampled;
    
};

#endif
