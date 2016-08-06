
#ifndef GLOW_SHADER_HPP
#define GLOW_SHADER_HPP

#include "Common.hpp"

class Shader {
public:
    
    Shader();
    ~Shader();
    
    Shader(Shader const &) = delete;
    Shader & operator=(Shader const &) = delete;
    
    GLuint getHandle() const;
    
    bool addSource(GLenum type, std::string const & code);
    bool addSourceFile(GLenum type, std::string const & path);
    
    bool link();
    
    void use();
    
    GLint getUniformLocation(std::string const & name);
    
    void setUniform(GLint location, float x);
    void setUniform(std::string const & name, float x);
    
    void setUniform(GLint location, GLint x);
    void setUniform(std::string const & name, GLint x);
    
    void setUniform(GLint location, glm::vec2 const & v);
    void setUniform(std::string const & name, glm::vec2 const & v);
    
    void setUniform(GLint location, glm::vec3 const & v);
    void setUniform(std::string const & name, glm::vec3 const & v);
    
    void setUniform(GLint location, glm::vec4 const & v);
    void setUniform(std::string const & name, glm::vec4 const & v);
    
    void setUniform(GLint location, glm::mat4 const & m);
    void setUniform(std::string const & name, glm::mat4 const & m);
    
private:

    GLuint handle;
    std::map<std::string, GLint> uniforms;
    
};

#endif
