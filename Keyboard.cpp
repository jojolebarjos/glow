
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

boolx Keyboard::getButton(uint32_t id) const {
    return id <= GLFW_KEY_LAST ? boolx(button[current ^ 1][id] == GLFW_PRESS, button[current][id] == GLFW_PRESS) : boolx();
}

boolx Keyboard::getPrimaryButton() const {
    return getButton(GLFW_KEY_SPACE);
}

boolx Keyboard::getSecondaryButton() const {
    return getButton(GLFW_KEY_ENTER);
}

void Keyboard::update() {
    current ^= 1;
    for (int i = 0; i <= GLFW_KEY_LAST; ++i)
        button[current][i] = glfwGetKey(window->getHandle(), i);
}
