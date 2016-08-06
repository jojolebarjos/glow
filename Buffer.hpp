
#ifndef GLOW_BUFFER_HPP
#define GLOW_BUFFER_HPP

#include "Common.hpp"

class Buffer {
public:
    
    Buffer();
    ~Buffer();
    
    Buffer(Buffer const &) = delete;
    Buffer & operator=(Buffer const &) = delete;
    
    GLuint getHandle() const;
    
    void bind(GLenum target);
    
    void setData(GLuint size, void const * pointer, GLenum usage);
    void setSubData(GLuint offset, GLuint size, void const * pointer);
    void getSubData(GLuint offset, GLuint size, void * pointer);
    
    void * map(GLbitfield access = GL_MAP_WRITE_BIT);
    void * map(GLintptr offset, GLsizeiptr size, GLbitfield access = GL_MAP_WRITE_BIT);
    void unmap();
    
private:
    
    GLuint handle;
    GLenum target;

};

#endif
