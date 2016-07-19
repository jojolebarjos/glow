
#include "Common.hpp"
#include "Shader.hpp"
#include "Image.hpp"
#include "Texture2D.hpp"
#include "Mesh.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"

// Debug context seems to cause issues with gDebugger :/
//#define DEBUG_CONTEXT

#ifdef DEBUG_CONTEXT
static void APIENTRY debug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const * message, void const * userParam) {
    std::cout << glGetFriendlyName(source) << ':' << glGetFriendlyName(type) << ':' << glGetFriendlyName(severity) << ' ' << message << std::endl;
    // TODO if we use that, is shader log still useful?
}
#endif

int main(int argc, char** argv) {
    
    // Initialize GLFW
    if (!glfwInit())
        return -1;
    
    // Create window
#ifdef DEBUG_CONTEXT
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow * window = glfwCreateWindow(640, 480, "Glow", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    // Load GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -2;
    }
    
    // Enable OpenGL debugging
#ifdef DEBUG_CONTEXT
    glEnable(GL_DEBUG_OUTPUT);
    // TODO glDebugMessageControl?
    glDebugMessageCallback(debug, nullptr);
#endif
    
    // Get screen size
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
    // Game scope
    {
        
        // Load simple image
        Image image;
        image.load("Test.bmp");
        
        // Create simple texture
        Texture2D texture;
        texture.bind(0);
        texture.create(image);
        texture.setInterpolation(true);
        
        // Load depth-only rendering shader
        Shader depth_shader;
        depth_shader.addSourceFile(GL_VERTEX_SHADER, "Depth.vs");
        depth_shader.addSourceFile(GL_FRAGMENT_SHADER, "Depth.fs");
        depth_shader.link();
        
        /*
        // Load shadow volume extrusion shader
        Shader extrusion_shader;
        extrusion_shader.addSourceFile(GL_VERTEX_SHADER, "Extrusion.vs");
        extrusion_shader.addSourceFile(GL_GEOMETRY_SHADER, "Extrusion.gs");
        extrusion_shader.addSourceFile(GL_FRAGMENT_SHADER, "Extrusion.fs");
        extrusion_shader.link();
        */
        
        // Load shading shader
        Shader shading_shader;
        shading_shader.addSourceFile(GL_VERTEX_SHADER, "Shading.vs");
        shading_shader.addSourceFile(GL_FRAGMENT_SHADER, "Shading.fs");
        shading_shader.link();
        
        // Load texture shader
        Shader texture_shader;
        texture_shader.addSourceFile(GL_VERTEX_SHADER, "Texture.vs");
        texture_shader.addSourceFile(GL_FRAGMENT_SHADER, "Texture.fs");
        texture_shader.link();
        
        // Load simple mesh
        Mesh mesh;
        mesh.load("Cube.obj");
        
        // Create simple buffer
        Buffer buffer;
        buffer.bind(GL_ARRAY_BUFFER);
        buffer.setData(mesh.getCount() * 4 * (3 + 3 + 2), nullptr, GL_STATIC_DRAW);
        buffer.setSubData(0, mesh.getCount() * 4 * 3, mesh.getPositions());
        buffer.setSubData(mesh.getCount() * 4 * 3, mesh.getCount() * 4 * 3, mesh.getNormals());
        buffer.setSubData(mesh.getCount() * 4 * (3 + 3), mesh.getCount() * 4 * 2, mesh.getCoordinates());
        
        // Create simple vertex array object
        VertexArray array;
        array.bind();
        array.addAttribute(0, 3, GL_FLOAT, 0, 0);
        array.addAttribute(1, 3, GL_FLOAT, 0, mesh.getCount() * 4 * 3);
        array.addAttribute(2, 2, GL_FLOAT, 0, mesh.getCount() * 4 * (3 + 3));
        
        // Initialize camera matrices
        glm::mat4 projection = glm::perspective(PI / 3.0f, (float)width / (float)height, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(-2.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 model = glm::mat4();
        
        // Always use depth test
        glEnable(GL_DEPTH_TEST);
        
        // Game loop
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            float time = glfwGetTime();
            view = glm::lookAt(glm::vec3(glm::cos(time / 3) * 2.0f, glm::sin(time / 3) * 2.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            //model = glm::rotate(time, glm::vec3(1, 1, 1));
            
            // Clear everything
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            
            // Select depth shader
            depth_shader.use();
            depth_shader.setUniform("projection", projection);
            depth_shader.setUniform("view", view);
            
            // Draw geometry
            depth_shader.setUniform("model", model);
            glDrawArrays(GL_TRIANGLES, 0, mesh.getCount());
            
            // Do not override old depth
            glDepthMask(GL_FALSE);
            
            // Enable stencil to render shadows
            //glEnable(GL_STENCIL_TEST);
            
            // For each light...
            {
                glm::vec3 light_position(1, 1, 2);
                glm::vec3 light_color(1, 0.8, 0.2);
                float light_radius(3);
                
                // Clear stencil
                glClear(GL_STENCIL_BUFFER_BIT);
                
                // Use Carmack's reverse shadow volume strategy
                glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_ALWAYS, 0, ~(GLint)0);
                glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR, GL_KEEP);
                glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR, GL_KEEP);
                
                // Select extrusion shader
                // TODO
                
                // Draw geometry
                //glDrawArrays(GL_TRIANGLES, 0, mesh.getCount());
                
                // Use stencil to only draw on non-zero area
                glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_NOTEQUAL, 0, ~(GLint)0);
                glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
                
                // Render only fragments that are exactly on geometry
                glDepthFunc(GL_EQUAL);
                
                // Use additive blend to combine lightmaps
                glEnable(GL_BLEND);
                glBlendEquation(GL_ADD);
                glBlendFunc(GL_ONE, GL_ONE);
                
                // Select shading shader
                shading_shader.use();
                shading_shader.setUniform("projection", projection);
                shading_shader.setUniform("view", view);
                shading_shader.setUniform("light_position", light_position);
                shading_shader.setUniform("light_color", light_color);
                shading_shader.setUniform("light_radius", light_radius);
                
                // Draw geometry again to shade surfaces properly
                shading_shader.setUniform("model", model);
                glDrawArrays(GL_TRIANGLES, 0, mesh.getCount());
                
                // Restore default values
                glDisable(GL_BLEND);
                glDepthFunc(GL_LESS);
            }
            
            // Disable stencil
            glDisable(GL_STENCIL_TEST);
            
            // Render only fragments that are exactly on geometry
            glDepthFunc(GL_EQUAL);
            
            // Use multiplicative blend to combine light and texture
            glEnable(GL_BLEND);
            glBlendEquation(GL_ADD);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            
            // Select texture shader
            texture_shader.use();
            texture_shader.setUniform("projection", projection);
            texture_shader.setUniform("view", view);
            texture_shader.setUniform("texture", 0);
            
            // Draw geometry again zo have textures
            texture_shader.setUniform("model", model);
            glDrawArrays(GL_TRIANGLES, 0, mesh.getCount());
            
            // Restore defaults
            glDisable(GL_BLEND);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            
            // Swap buffers
            glfwSwapBuffers(window);
        }
    }
    
    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
