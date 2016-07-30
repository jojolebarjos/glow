
#include "Smoke.hpp"
#include "Mesh.hpp"
#include "Buffer.hpp"

Smoke::Smoke(Window * window) : window(window) {}

bool Smoke::initialize() {
    
    // Load shaders
    pass1.addSourceFile(GL_VERTEX_SHADER, "Smoke.vs");
    pass1.addSourceFile(GL_FRAGMENT_SHADER, "Smoke1.fs");
    pass2.addSourceFile(GL_VERTEX_SHADER, "Smoke.vs");
    pass2.addSourceFile(GL_FRAGMENT_SHADER, "Smoke2.fs");
    pass3.addSourceFile(GL_VERTEX_SHADER, "Smoke.vs");
    pass3.addSourceFile(GL_FRAGMENT_SHADER, "Smoke3.fs");
    render.addSourceFile(GL_VERTEX_SHADER, "Smoke.vs");
    render.addSourceFile(GL_FRAGMENT_SHADER, "Smoke4.fs");
    if (!pass1.link() || !pass2.link() || !pass3.link() || !render.link()) {
        std::cout << "Failed to compile shaders" << std::endl;
        return false;
    }
    
    // Load square
    if (!mesh.load("Square.obj")) {
        std::cout << "Failed to load Square.obj" << std::endl;
        return false;
    }
    buffer.bind(GL_ARRAY_BUFFER);
    buffer.setData(mesh.getCount() * 4 * (3 + 2), nullptr, GL_STATIC_DRAW);
    buffer.setSubData(0, mesh.getCount() * 4 * 3, mesh.getPositions());
    buffer.setSubData(mesh.getCount() * 4 * 3, mesh.getCount() * 4 * 2, mesh.getCoordinates());
    array.bind();
    array.addAttribute(0, 3, GL_FLOAT, 0, 0);
    array.addAttribute(1, 2, GL_FLOAT, 0, mesh.getCount() * 4 * 3);
    
    // Create textures
    for (int i = 0; i < 2; ++i) {
        textures[i].createColor(window->getWidth(), window->getHeight(), true);
        textures[i].setInterpolation(true);
        textures[i].setBorder(false);
        framebuffers[i].bind();
        framebuffers[i].attach(textures[i]);
        framebuffers[i].validate();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    // Ready
    current = 0;
    return true;
}

void Smoke::update() {
    // TODO add VR support (draw on the ground)
    
    // Bind objects
    array.bind();
    textures[0].bind(0);
    textures[1].bind(1);

    // Compute effect
    float dt = window->getDeltaTime();
    if (dt > 0.0001f) {
        glm::vec2 new_mouse = window->getMouseLocation();
        glm::vec2 delta = new_mouse - last_mouse;
        delta *= 0.2f;
        last_mouse = new_mouse;
        average_delta = average_delta * 0.9f + delta * 0.1f / window->getDeltaTime();
    }
    int mode = window->isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) ? 1 : window->isMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT) ? 2 : 0;
    
    // Apply forces
    pass1.use();
    pass1.setUniform("mode", mode);
    pass1.setUniform("location", window->getMouseLocation());
    pass1.setUniform("radius", 16.0f);
    pass1.setUniform("direction", average_delta * window->getDeltaTime());
    pass1.setUniform("previous", current);
    current ^= 1;
    framebuffers[current].bind();
    glDrawArrays(GL_TRIANGLES, 0, mesh.getCount());

    // Compute pressure
    pass2.use();
    for (int i = 0; i < 30; ++i) {
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
    render.setUniform("mode", window->isKeyboardButtonDown(GLFW_KEY_SPACE) ? 0 : 1);
    glDrawArrays(GL_TRIANGLES, 0, mesh.getCount());
}
