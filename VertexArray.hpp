
#ifndef GLOW_VERTEXARRAY_HPP
#define GLOW_VERTEXARRAY_HPP

#include "Common.hpp"

class VertexArray {
public:
    
    VertexArray();
    ~VertexArray();
    
    VertexArray(VertexArray const &) = delete;
    VertexArray & operator=(VertexArray const &) = delete;
    
    void bind();
    
    void addAttribute(int index, GLint size, GLenum type, GLuint stride, GLuint offset, bool instanced = false);
    
    // Note: one location can hold up to 4 floats, i.e. mat4 takes 4 slots
    void addAttributeMat4(int index, GLuint stride, GLuint offset, bool instanced = false);
    
private:

    GLuint handle;
    
};

#endif
