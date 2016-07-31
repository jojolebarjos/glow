
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

class Renderer {
public:
    
    // TODO hide these structs?
    
    struct LightInfo {
        glm::vec3 position;
        float radius;
        glm::vec3 color;
    };
    
    struct MeshInfo {
        glm::mat4 transform;
        GLuint mesh;
        GLuint color;
        // TODO light, normal, material...
    };
    
    Renderer(Window * window);
    ~Renderer();
    
    Renderer(Renderer const &) = delete;
    Renderer & operator=(Renderer const &) = delete;
    
    bool initialize(uint32_t width, uint32_t height);
    
    GLuint loadMesh(std::string const & path);
    
    GLuint loadImage(std::string const & path);
    
    void pack();
    
    void addLight(LightInfo const & light);
    
    void addMesh(MeshInfo const & mesh);
    
    void clear();
    
    void render(GLuint framebuffer, glm::mat4 const & projection, glm::mat4 const & view);
    
private:
    
    void drawUntexturedObjects(Shader & shader);
    void drawCasterObjects(Shader & shader);
    void drawTexturedObjects(Shader & shader);
    void drawSquare();
    
    Window * window;
    uint32_t width;
    uint32_t height;
    
    // TODO maybe this mapping should not be done here?
    std::map<std::string, GLuint> meshNames;
    std::map<std::string, GLuint> imageNames;
    
    std::vector<Mesh> meshDatas;
    std::vector<Image> imageDatas;
    
    std::vector<glm::ivec2> meshMaps;
    std::vector<glm::ivec2> imageMaps;
    
    Buffer buffer;
    VertexArray array;
    // TODO use texture array
    std::vector<Texture *> textures;
    
    std::vector<LightInfo> lights;
    std::vector<MeshInfo> meshes;
    // TODO other temporary buffers (particles, ...)
    
    glm::mat4 projection;
    glm::mat4 view;
    
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
