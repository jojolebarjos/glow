
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
    
    void addAttribute(int index, GLint size, GLenum type, GLuint stride, GLuint offset);
    
private:

    GLuint handle;
    
};

#endif
