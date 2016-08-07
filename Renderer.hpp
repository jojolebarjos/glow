
#ifndef GLOW_RENDERER_HPP
#define GLOW_RENDERER_HPP

#include "Common.hpp"
#include "Buffer.hpp"
#include "Image.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "Shader.hpp"
#include "Framebuffer.hpp"
#include "Window.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "Light.hpp"
#include "Material.hpp"

class Renderer {
public:
    
    Renderer(Window * window);
    ~Renderer();
    
    Renderer(Renderer const &) = delete;
    Renderer & operator=(Renderer const &) = delete;
    
    bool initialize(uint32_t width, uint32_t height);
    
    uint32_t loadMesh(std::string const & path);
    uint32_t loadImage(std::string const & path);
    void pack();
    
    void addLight(Light const * light);
    void addModel(Model const * model);
    void clear();
    
    void render(Camera const * camera);
    
private:
    
    Window * window;
    uint32_t width;
    uint32_t height;
    
    // TODO maybe this mapping should not be done here?
    std::map<std::string, uint32_t> meshNames;
    std::map<std::string, uint32_t> imageNames;
    
    std::vector<Mesh> meshDatas;
    std::vector<Image> imageDatas;
    
    std::vector<glm::ivec2> meshMaps;
    std::vector<glm::ivec2> imageMaps;
    
    Buffer geometry;
    Buffer permodel;
    VertexArray array;
    Texture * textures;
    
    std::vector<Light const *> lights;
    std::vector<Model const *> models;
    // TODO other temporary buffers (particles, ...)
    
    Shader render_shader;
    Shader extrusion_shader;
    Shader shading_shader;
    Shader finalize_shader;
    Shader antialiasing_shader;
    
    Texture render_color;
    Texture render_position;
    Texture render_normal;
    Texture render_light;
    Texture render_depthStencil;
    Framebuffer render_framebuffer;
    Framebuffer render_light_framebuffer;
    
    Texture processing_color[3];
    Framebuffer processing_framebuffer[3];
    
};

#endif
