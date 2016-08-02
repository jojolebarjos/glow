
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
    render_3d.addSourceFile(GL_VERTEX_SHADER, "Render.vs");
    render_3d.addSourceFile(GL_FRAGMENT_SHADER, "Smoke4.fs");
    if (!pass1.link() || !pass2.link() || !pass3.link() || !render.link() || !render_3d.link()) {
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
    array.addAttribute(2, 2, GL_FLOAT, 0, mesh.getCount() * 4 * 3);
    
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

    // Bind objects
    glViewport(0, 0, window->getWidth(), window->getHeight());
    array.bind();
    textures[0].bind(0);
    textures[1].bind(1);

    // Get input
    int mode = 0;
    glm::vec2 new_location = location;
    if (window->getHead()) {
        glm::mat4 transform = window->getController(0)->getTransform();
        glm::vec3 position(transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        glm::vec3 forward(transform * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
        if (forward.z < -0.1f) {
            float distance = -position.z / forward.z;
            new_location = glm::vec2(position + forward * distance);
            new_location.x = (new_location.x + 2.0f) * 0.25f * window->getWidth();
            new_location.y = (new_location.y + 2.0f) * 0.25f * window->getHeight();
        }
        if (window->getController(0)->getPrimaryButton())
            mode = 2;
        else if (window->getController(0)->getSecondaryButton())
            mode = 1;
    } else {
        new_location = window->getMouse()->getPosition();
        if (window->getMouse()->getPrimaryButton())
            mode = 1;
        else if (window->getMouse()->getSecondaryButton())
            mode = 2;
    }
    
    // Compute effect
    if (window->getDeltaTime() > 0.0001f) {
        glm::vec2 delta = new_location - location;
        delta *= 0.2f;
        direction = direction * 0.9f + delta * 0.1f / window->getDeltaTime();
    }
    location = new_location;
    
    // Apply forces
    pass1.use();
    pass1.setUniform("mode", mode);
    pass1.setUniform("location", location);
    pass1.setUniform("radius", 16.0f);
    pass1.setUniform("direction", direction * window->getDeltaTime());
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
    if (window->getHead()) {
        // TODO improve 3D rendering (antialiasing, larger area, show controller...)
        glViewport(0, 0, window->getHead()->getWidth(), window->getHead()->getHeight());
        render_3d.use();
        render_3d.setUniform("previous", current);
        render_3d.setUniform("mode", window->getKeyboard()->getButton(GLFW_KEY_SPACE) ? 0 : 1);
        render_3d.setUniform("model", glm::mat4(2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
        for (int i = 0; i < 2; ++i) {
            window->getHead()->getEye(i)->getFramebuffer()->bind();
            glClear(GL_COLOR_BUFFER_BIT);
            render_3d.setUniform("projection", window->getHead()->getEye(i)->getProjection());
            render_3d.setUniform("view", window->getHead()->getEye(i)->getView());
            glDrawArrays(GL_TRIANGLES, 0, mesh.getCount());
        }
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        render.use();
        render.setUniform("previous", current);
        render.setUniform("mode", window->getKeyboard()->getButton(GLFW_KEY_SPACE) ? 0 : 1);
        glDrawArrays(GL_TRIANGLES, 0, mesh.getCount());
    }
}
