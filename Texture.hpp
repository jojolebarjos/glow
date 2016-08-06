
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
    
    // Note: can only be called once, and should not be bound before
    void createColor(Image const & image, bool mipmapped = false);
    void createColor(uint32_t width, uint32_t height, bool floating = false, GLuint multisampling = 0);
    void createDepthStencil(uint32_t width, uint32_t height, GLuint multisampling = 0);
    void createColorArray(std::vector<Image const *> images, bool mipmapped = false);
    
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint32_t getDepth() const; // Note: zero for non-array textures
    
    bool isArray() const;
    bool isMipmapped() const;
    bool isDepthStencil() const;
    GLuint getMultisampling() const; // Note: zero for non-multisampled textures
    
    void bind();
    void bind(int slot);
    
    void setInterpolation(bool linear);
    
    void setAnisotropy(bool enabled);
    
    void setBorder(bool clamp);
    void setBorder(glm::vec4 const & color);
    
    // TODO upload/download data? This would require to recompute mipmaps (i.e. individually for arrays...)
    
private:
    
    GLuint handle;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    bool mipmapped;
    bool depthStencil;
    GLuint multisampling;
    
    // TODO use glTexStorage instead? https://www.opengl.org/wiki/Common_Mistakes#Creating_a_complete_texture
    // TODO http://stackoverflow.com/questions/12372058/how-to-use-gl-texture-2d-array-in-opengl-3-2
    
};

#endif
