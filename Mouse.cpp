
#include "Mouse.hpp"
#include "Window.hpp"

Mouse::Mouse(Window * window) : window(window), current(0) {
    for (int i = 0; i <= GLFW_MOUSE_BUTTON_LAST; ++i)
        button[0][i] = button[1][i] = GLFW_RELEASE;
}
    
bool Mouse::isConnected() const {
    return true;
}

uint32_t Mouse::getAxisCount() const {
    return 2;
}

float Mouse::getAxisValue(uint32_t id) const {
    return id == 0 ? position.x : id == 1 ? position.y : 0.0f;
}

uint32_t Mouse::getButtonCount() const {
    return GLFW_MOUSE_BUTTON_LAST + 1;
}

bool Mouse::isButtonDown(uint32_t id) const {
    return id <= GLFW_MOUSE_BUTTON_LAST ? button[current][id] == GLFW_PRESS : false;
}

bool Mouse::isButtonPressed(uint32_t id) const {
    return id <= GLFW_MOUSE_BUTTON_LAST ? button[current ^ 1][id] == GLFW_RELEASE && button[current][id] == GLFW_PRESS : false;
}

bool Mouse::isButtonReleased(uint32_t id) const {
    return id <= GLFW_MOUSE_BUTTON_LAST ? button[current ^ 1][id] == GLFW_PRESS && button[current][id] == GLFW_RELEASE : false;
}

glm::vec2 Mouse::getPosition() const {
    return position;
}

void Mouse::update() {
    double x, y;
    glfwGetCursorPos(window->getHandle(), &x, &y);
    position.x = x;
    position.y = window->getHeight() - y;
    current ^= 1;
    for (int i = 0; i <= GLFW_MOUSE_BUTTON_LAST; ++i)
        button[current][i] = glfwGetMouseButton(window->getHandle(), i);
}
