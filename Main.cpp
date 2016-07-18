
#include "Common.hpp"
#include "Shader.hpp"
#include "Image.hpp"
#include "Texture2D.hpp"
#include "Mesh.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"

int main(int argc, char** argv) {
    if (!glfwInit())
        return -1;
    GLFWwindow * window = glfwCreateWindow(640, 480, "Glow", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -2;
    }
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
        mesh.load("Square.obj");
        
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
        
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            
            glDrawArrays(GL_TRIANGLES, 0, mesh.getCount());
            
            glfwSwapBuffers(window);
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

