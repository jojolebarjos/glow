
#include "Smoke.hpp"
#include "Mesh.hpp"
#include "Buffer.hpp"

Smoke::Smoke(GLFWwindow * window) : window(window) {}

bool Smoke::initialize() {
    // TODO check errors
    
    // Load shaders
    pass1.addSourceFile(GL_VERTEX_SHADER, "Smoke.vs");
    pass1.addSourceFile(GL_FRAGMENT_SHADER, "Smoke1.fs");
    pass1.link();
    pass2.addSourceFile(GL_VERTEX_SHADER, "Smoke.vs");
    pass2.addSourceFile(GL_FRAGMENT_SHADER, "Smoke2.fs");
    pass2.link();
    pass3.addSourceFile(GL_VERTEX_SHADER, "Smoke.vs");
    pass3.addSourceFile(GL_FRAGMENT_SHADER, "Smoke3.fs");
    pass3.link();
    render.addSourceFile(GL_VERTEX_SHADER, "Smoke.vs");
    render.addSourceFile(GL_FRAGMENT_SHADER, "Smoke4.fs");
    render.link();
    
    // Load quad
    mesh.load("Square.obj");
    buffer.bind(GL_ARRAY_BUFFER);
    buffer.setData(mesh.getCount() * 4 * (3 + 2), nullptr, GL_STATIC_DRAW);
    buffer.setSubData(0, mesh.getCount() * 4 * 3, mesh.getPositions());
    buffer.setSubData(mesh.getCount() * 4 * 3, mesh.getCount() * 4 * 2, mesh.getCoordinates());
    array.bind();
    array.addAttribute(0, 3, GL_FLOAT, 0, 0);
    array.addAttribute(1, 2, GL_FLOAT, 0, mesh.getCount() * 4 * 3);
    
    // Get screen size
    glfwGetFramebufferSize(window, &width, &height);
    glm::vec2 size(width, height);
    pass1.use();
    pass1.setUniform("size", size);
    pass2.use();
    pass2.setUniform("size", size);
    pass3.use();
    pass3.setUniform("size", size);
    render.use();
    render.setUniform("size", size);
    
    // Create textures
    for (int i = 0; i < 2; ++i) {
        textures[i].createColor(width, height, true);
        textures[i].setInterpolation(true);
        textures[i].setBorder(false);
        framebuffers[i].bind();
        framebuffers[i].attach(textures[i]);
        framebuffers[i].validate();
        glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    // Ready
    current = 0;
    return true;
}

void Smoke::update() {
    
    // Bind objects
    array.bind();
    textures[0].bind(0);
    textures[1].bind(1);

    // Apply forces
    int mode = glfwGetMouseButton(window, 0) == GLFW_PRESS ? 1 : glfwGetMouseButton(window, 1) == GLFW_PRESS ? 2 : 0;
    pass1.use();
    pass1.setUniform("mode", mode);
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    pass1.setUniform("location", glm::vec2(x, height - y));
    pass1.setUniform("radius", 16.0f);
    pass1.setUniform("previous", current);
    current ^= 1;
    framebuffers[current].bind();
    glDrawArrays(GL_TRIANGLES, 0, mesh.getCount());

    // Compute pressure
    pass2.use();
    for (int i = 0; i < 50; ++i) {
        pass2.setUniform("previous", current);
        current ^= 1;
        framebuffers[current].bind();
        glDrawArrays(GL_TRIANGLES, 0, mesh.getCount());
    }

    // Solve velocities
    pass3.use();
    pass3.setUniform("previous", current);
    current ^= 1;
    framebuffers[current].bind();
    glDrawArrays(GL_TRIANGLES, 0, mesh.getCount());

    // Render fluid
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    render.use();
    render.setUniform("previous", current);
    render.setUniform("mode", glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ? 0 : 1);
    glDrawArrays(GL_TRIANGLES, 0, mesh.getCount());
}
