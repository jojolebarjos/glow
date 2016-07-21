
#include "Common.hpp"
#include "Scene.hpp"
#include "Smoke.hpp"

extern "C" {
#include <jpeglib.h>
}

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
    std::cout << "libjpeg: " << (JPEG_LIB_VERSION / 10) << (char)(JPEG_LIB_VERSION % 10 + 'a' - 1) << std::endl;
    std::cout << "Bullet: " << (BT_BULLET_VERSION / 100) << '.' << ((BT_BULLET_VERSION / 10) % 10) << '.' << (BT_BULLET_VERSION % 10) << std::endl;
    
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
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;
    
    // Game loop
    {
        // TODO choose the desired game from config file/arguments
        Scene
        //Smoke
        game(window);
        game.initialize();
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            game.update();
            glfwSwapBuffers(window);
        }
    }
    
    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
