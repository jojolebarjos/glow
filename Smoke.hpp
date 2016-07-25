
#ifndef GLOW_SMOKE_HPP
#define GLOW_SMOKE_HPP

#include "Common.hpp"
#include "Texture.hpp"
#include "Framebuffer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "Mesh.hpp"
#include "Buffer.hpp"

class Smoke {
public:
    
    // TODO need to clean the shaders :3
    
    Smoke(GLFWwindow * window);
    
    bool initialize();
    void update();
    
private:
    
    GLFWwindow * window;
    int width;
    int height;

    Shader pass1;
    Shader pass2;
    Shader pass3;
    Shader render;
    
    Texture textures[2];
    Framebuffer framebuffers[2];
    
    Mesh mesh;
    Buffer buffer;
    VertexArray array;
    
    int current;
    
};

#endif /* SMOKE_HPP */

