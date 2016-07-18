
#ifndef GLOW_SHADER_HPP
#define GLOW_SHADER_HPP

#include "Common.hpp"

class Shader {
public:
    
    Shader();
    ~Shader();
    
    Shader(Shader const &) = delete;
    Shader & operator=(Shader const &) = delete;
    
    bool addSource(GLenum type, std::string const & code);
    bool addSourceFile(GLenum type, std::string const & path);
    
    bool link();
    
    void use();
    
    GLint getUniformLocation(std::string const & name);
    
    void setUniform(GLint location, float x);
    void setUniform(std::string const & name, float x);
    
    void setUniform(GLint location, GLint x);
    void setUniform(std::string const & name, GLint x);
    
    // TODO others...
    
    void setUniform(GLint location, glm::mat4 const & m);
    void setUniform(std::string const & name, glm::mat4 const & m);
    
private:

    GLuint handle;
    std::map<std::string, GLint> uniforms;
    
};

#endif
