
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
        
        // Load shadow volume extrusion shader
        Shader extrusion_shader;
        extrusion_shader.addSourceFile(GL_VERTEX_SHADER, "Extrusion.vs");
        extrusion_shader.addSourceFile(GL_GEOMETRY_SHADER, "Extrusion.gs");
        extrusion_shader.addSourceFile(GL_FRAGMENT_SHADER, "Extrusion.fs");
        extrusion_shader.link();
        
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
        
        // Load cube mesh
        Mesh cube_mesh;
        cube_mesh.load("Cube.obj");
        
        // Create cube buffer
        Buffer cube_buffer;
        cube_buffer.bind(GL_ARRAY_BUFFER);
        cube_buffer.setData(cube_mesh.getCount() * 4 * (3 + 3 + 2), nullptr, GL_STATIC_DRAW);
        cube_buffer.setSubData(0, cube_mesh.getCount() * 4 * 3, cube_mesh.getPositions());
        cube_buffer.setSubData(cube_mesh.getCount() * 4 * 3, cube_mesh.getCount() * 4 * 3, cube_mesh.getNormals());
        cube_buffer.setSubData(cube_mesh.getCount() * 4 * (3 + 3), cube_mesh.getCount() * 4 * 2, cube_mesh.getCoordinates());
        
        // Create cube vertex array object
        VertexArray cube_array;
        cube_array.bind();
        cube_array.addAttribute(0, 3, GL_FLOAT, 0, 0);
        cube_array.addAttribute(1, 3, GL_FLOAT, 0, cube_mesh.getCount() * 4 * 3);
        cube_array.addAttribute(2, 2, GL_FLOAT, 0, cube_mesh.getCount() * 4 * (3 + 3));
        
        // Load square mesh for cube
        Mesh square_mesh;
        square_mesh.load("Square.obj");
        
        // Create square buffer
        Buffer square_buffer;
        square_buffer.bind(GL_ARRAY_BUFFER);
        square_buffer.setData(square_mesh.getCount() * 4 * (3 + 3 + 2), nullptr, GL_STATIC_DRAW);
        square_buffer.setSubData(0, square_mesh.getCount() * 4 * 3, square_mesh.getPositions());
        square_buffer.setSubData(square_mesh.getCount() * 4 * 3, square_mesh.getCount() * 4 * 3, square_mesh.getNormals());
        square_buffer.setSubData(square_mesh.getCount() * 4 * (3 + 3), square_mesh.getCount() * 4 * 2, square_mesh.getCoordinates());
        
        // Create square vertex array object
        VertexArray square_array;
        square_array.bind();
        square_array.addAttribute(0, 3, GL_FLOAT, 0, 0);
        square_array.addAttribute(1, 3, GL_FLOAT, 0, square_mesh.getCount() * 4 * 3);
        square_array.addAttribute(2, 2, GL_FLOAT, 0, square_mesh.getCount() * 4 * (3 + 3));
        
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
            view = glm::lookAt(glm::vec3(glm::cos(time / 3) * 3.0f, glm::sin(time / 3) * 3.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            //model = glm::rotate(time, glm::vec3(1, 1, 1));
            
            // TODO might be able to enable face culling at some point?
            
            // Clear everything
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            
            // Geometry helper
            #define DRAW_GEOMETRY(shader) \
                shader.setUniform("model", model); \
                cube_array.bind(); \
                glDrawArrays(GL_TRIANGLES, 0, cube_mesh.getCount()); \
                shader.setUniform("model", model); \
                square_array.bind(); \
                glDrawArrays(GL_TRIANGLES, 0, square_mesh.getCount());
            
            // Select depth shader
            depth_shader.use();
            depth_shader.setUniform("projection", projection);
            depth_shader.setUniform("view", view);
            
            // Draw geometry
            DRAW_GEOMETRY(depth_shader);
            
            // Do not override old depth
            glDepthMask(GL_FALSE);
            
            // Enable stencil to render shadows
            glEnable(GL_STENCIL_TEST);
            
            // For each light...
            {
                glm::vec3 light_position(1, 1, 2);
                glm::vec3 light_color(1, 0.8, 0.2);
                float light_radius(5);
                
                // Clear stencil
                glClear(GL_STENCIL_BUFFER_BIT);
                
                // Use Carmack's reverse shadow volume strategy
                glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_ALWAYS, 0, ~(GLint)0);
                glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
                glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
                
                // Do not write color as well
                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                
                // Select extrusion shader
                extrusion_shader.use();
                extrusion_shader.setUniform("projection", projection);
                extrusion_shader.setUniform("view", view);
                extrusion_shader.setUniform("light_position", light_position);
                extrusion_shader.setUniform("light_radius", light_radius);
                
                // TODO depth clamp?
                // see https://www.opengl.org/wiki_132/index.php?title=Vertex_Post-Processing&redirect=no#Depth_clamping
                
                // Draw geometry
                DRAW_GEOMETRY(extrusion_shader);
                
                // Now, write color
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                
                // Use stencil to only draw on non-zero area
                glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_EQUAL, 0, ~(GLint)0);
                glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
                
                // Render only fragments that are exactly on geometry
                glDepthFunc(GL_EQUAL);
                
                // Use additive blend to combine lightmaps
                glEnable(GL_BLEND);
                glBlendEquation(GL_FUNC_ADD);
                glBlendFunc(GL_ONE, GL_ONE);
                
                // Select shading shader
                shading_shader.use();
                shading_shader.setUniform("projection", projection);
                shading_shader.setUniform("view", view);
                shading_shader.setUniform("light_position", light_position);
                shading_shader.setUniform("light_color", light_color);
                shading_shader.setUniform("light_radius", light_radius);
                
                // Draw geometry again to shade surfaces properly
                DRAW_GEOMETRY(shading_shader);
                
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
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            
            // Select texture shader
            texture_shader.use();
            texture_shader.setUniform("projection", projection);
            texture_shader.setUniform("view", view);
            texture_shader.setUniform("texture", 0);
            
            // Draw geometry again zo have textures
            DRAW_GEOMETRY(texture_shader);
            
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
