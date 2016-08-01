
#include "Keyboard.hpp"
#include "Window.hpp"

Keyboard::Keyboard(Window * window) : window(window), current(0) {
    for (int i = 0; i <= GLFW_KEY_LAST; ++i)
        button[0][i] = button[1][i] = GLFW_RELEASE;
}

bool Keyboard::isConnected() const {
    return true;
}

uint32_t Keyboard::getButtonCount() const {
    return GLFW_KEY_LAST + 1;
}

bool Keyboard::isButtonDown(uint32_t id) const {
    return id <= GLFW_KEY_LAST ? button[current][id] == GLFW_PRESS : false;
}

bool Keyboard::isButtonPressed(uint32_t id) const {
    return id <= GLFW_KEY_LAST ? button[current ^ 1][id] == GLFW_RELEASE && button[current][id] == GLFW_PRESS : false;
}

bool Keyboard::isButtonReleased(uint32_t id) const {
    return id <= GLFW_KEY_LAST ? button[current ^ 1][id] == GLFW_PRESS && button[current][id] == GLFW_RELEASE : false;
}

void Keyboard::update() {
    current ^= 1;
    for (int i = 0; i <= GLFW_KEY_LAST; ++i)
        button[current][i] = glfwGetKey(window->getHandle(), i);
}
