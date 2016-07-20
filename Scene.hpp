
#ifndef GLOW_SCENE_HPP
#define GLOW_SCENE_HPP

#include "Common.hpp"
#include "Shader.hpp"
#include "Image.hpp"
#include "Texture2D.hpp"
#include "Mesh.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"

class Scene {
public:
    
    Scene(GLFWwindow * window);
    
    Scene(Scene const &) = delete;
    Scene & operator=(Scene const &) = delete;
    
    bool initialize();
    void update();
    void render();
    
    void drawUntexturedObjects(Shader & shader);
    void drawCasterObjects(Shader & shader);
    void drawTexturedObjects(Shader & shader);
    
private:

    GLFWwindow * window;
    int width;
    int height;
    
    glm::mat4 projection;
    glm::mat4 view;
    
    struct Light {
        glm::vec3 position;
        glm::vec3 color;
        float radius;
    };
    std::vector<Light> lights;
    
    Image image;
    Texture2D texture;
    
    Mesh cube_mesh;
    Buffer cube_buffer;
    VertexArray cube_array;
    
    Mesh square_mesh;
    Buffer square_buffer;
    VertexArray square_array;
    
    Shader depth_shader;
    Shader extrusion_shader;
    Shader shading_shader;
    Shader texture_shader;
};

#endif
