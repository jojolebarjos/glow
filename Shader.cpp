
#include "Shader.hpp"

#include <cstdio>

Shader::Shader() {
    handle = glCreateProgram();
    assert(handle);
}

Shader::~Shader() {
    glDeleteProgram(handle);
}

bool Shader::addSource(GLenum type, std::string const & code) {
    GLuint id = glCreateShader(type);
    if (!id)
        return false; // TODO report this properly?
    char const * pointer = code.c_str();
    glShaderSource(id, 1, &pointer, NULL);
    glCompileShader(id);
    GLint compiled;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        GLchar * buffer = new GLchar[length];
        glGetShaderInfoLog(id, length, NULL, buffer);
        // TODO improve logging system
        std::cout << buffer << std::endl;
        delete[] buffer;
        glDeleteShader(id);
        return false;
    }
    glAttachShader(handle, id);
    glDeleteShader(id);
    return true;
}

bool Shader::addSourceFile(GLenum type, std::string const & path) {
    FILE * file = fopen(path.c_str(), "rb");
    if (!file)
        return false;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    std::string code(size, ' ');
    if (fread(&code[0], size, 1, file) != 1) {
        fclose(file);
        return false;
    }
    fclose(file);
    return addSource(type, code);
}

bool Shader::link() {
    uniforms.clear();
    glLinkProgram(handle);
    GLint linked;
    glGetProgramiv(handle, GL_LINK_STATUS, &linked);
    if (linked)
        return true;
    GLint length;
    glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);
    GLchar * buffer = new GLchar[length];
    glGetProgramInfoLog(handle, length, NULL, buffer);
    // TODO improve logging system
    std::cout << buffer << std::endl;
    delete[] buffer;
    return false;
}

void Shader::use() {
    glUseProgram(handle);
}

GLint Shader::getUniformLocation(std::string const & name) {
    auto it = uniforms.find(name);
    if (it != uniforms.end())
        return it->second;
    GLint location = glGetUniformLocation(handle, name.c_str());
    uniforms[name] = location;
    return location;
}

void Shader::setUniform(GLint location, float x) {
    glUniform1f(location, x);
}

void Shader::setUniform(std::string const & name, float x) {
    setUniform(getUniformLocation(name), x);
}

void Shader::setUniform(GLint location, GLint x) {
    glUniform1i(location, x);
}

void Shader::setUniform(std::string const & name, GLint x) {
    setUniform(getUniformLocation(name), x);
}

void Shader::setUniform(GLint location, glm::vec2 const & v) {
    glUniform2f(location, v.x, v.y);
}

void Shader::setUniform(std::string const & name, glm::vec2 const & v) {
    setUniform(getUniformLocation(name), v);
}

void Shader::setUniform(GLint location, glm::vec3 const & v) {
    glUniform3f(location, v.x, v.y, v.z);
}

void Shader::setUniform(std::string const & name, glm::vec3 const & v) {
    setUniform(getUniformLocation(name), v);
}

void Shader::setUniform(GLint location, glm::vec4 const & v) {
    glUniform4f(location, v.x, v.y, v.z, v.w);
}

void Shader::setUniform(std::string const & name, glm::vec4 const & v) {
    setUniform(getUniformLocation(name), v);
}

void Shader::setUniform(GLint location, glm::mat4 const & m) {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::setUniform(std::string const & name, glm::mat4 const & m) {
    setUniform(getUniformLocation(name), m);
}
