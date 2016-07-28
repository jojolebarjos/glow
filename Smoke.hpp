
#ifndef GLOW_SMOKE_HPP
#define GLOW_SMOKE_HPP

#include "Common.hpp"
#include "Buffer.hpp"
#include "Framebuffer.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "Window.hpp"

class Smoke {
public:
    
    // TODO need to clean the shaders :3
    
    Smoke(Window * window);
    
    bool initialize();
    void update();
    
private:
    
    Window * window;

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

