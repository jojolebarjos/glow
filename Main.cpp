
#include "Common.hpp"
#include "Shader.hpp"
#include "Image.hpp"
#include "Texture2D.hpp"
#include "Mesh.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"

// Debug context seems to cause issues with gDebugger :/
#define DEBUG_CONTEXT

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
        Image image;
        image.load("Test.bmp");
        
        Texture2D texture;
        texture.bind(0);
        texture.create(image);
        texture.setInterpolation(false);
        
        Shader shader;
        shader.addSourceFile(GL_VERTEX_SHADER, "Test.vs");
        shader.addSourceFile(GL_FRAGMENT_SHADER, "Test.fs");
        shader.link();
        shader.use();
        shader.setUniform("tex", 0);
        
        Mesh mesh;
        mesh.load("Cube.obj");
        
        Buffer buffer;
        buffer.bind(GL_ARRAY_BUFFER);
        buffer.setData(mesh.getCount() * 4 * (3 + 3 + 2), nullptr, GL_STATIC_DRAW);
        buffer.setSubData(0, mesh.getCount() * 4 * 3, mesh.getPositions());
        buffer.setSubData(mesh.getCount() * 4 * 3, mesh.getCount() * 4 * 3, mesh.getNormals());
        buffer.setSubData(mesh.getCount() * 4 * (3 + 3), mesh.getCount() * 4 * 2, mesh.getCoordinates());
        
        VertexArray array;
        array.bind();
        array.addAttribute(0, 3, GL_FLOAT, 0, 0);
        array.addAttribute(1, 3, GL_FLOAT, 0, mesh.getCount() * 4 * 3);
        array.addAttribute(2, 2, GL_FLOAT, 0, mesh.getCount() * 4 * (3 + 3));
        
        glm::mat4 projection = glm::perspective(PI / 3.0f, (float)width / (float)height, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(-2.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 model = glm::mat4();
        shader.setUniform("projection", projection);
        shader.setUniform("view", view);
        shader.setUniform("model", model);
        
        glEnable(GL_DEPTH_TEST);
        
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            float time = glfwGetTime();
            
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            
            view = glm::lookAt(glm::vec3(glm::cos(time) * 2.0f, glm::sin(time) * 2.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            shader.setUniform("view", view);
            
            glDrawArrays(GL_TRIANGLES, 0, mesh.getCount());
            
            glfwSwapBuffers(window);
        }
    }
    
    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
