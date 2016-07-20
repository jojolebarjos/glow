
#include "Common.hpp"
#include "Scene.hpp"

// Debug context seems to cause issues with gDebugger :/
//#define DEBUG_CONTEXT

#ifdef DEBUG_CONTEXT
static void APIENTRY debug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const * message, void const * userParam) {
    std::cout << glGetFriendlyName(source) << ':' << glGetFriendlyName(type) << ':' << glGetFriendlyName(severity) << ' ' << message << std::endl;
}
#endif

int main(int argc, char** argv) {
    
    // Print libraries infos
    std::cout << "GLFW: " << glfwGetVersionString() << std::endl;
    std::cout << "GLEW: " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "GLM: " << GLM_VERSION_MAJOR << '.' << GLM_VERSION_MINOR << '.' << GLM_VERSION_PATCH << '.' << GLM_VERSION_REVISION << std::endl;
    
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
    glDebugMessageCallback(debug, nullptr);
#endif
    
    // Print OpenGL infos
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    
    // Game loop
    {
        Scene scene(window);
        scene.initialize();
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            scene.update();
            scene.render();
            glfwSwapBuffers(window);
        }
    }
    
    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
