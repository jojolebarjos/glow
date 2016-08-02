
#include "Joystick.hpp"

Joystick::Joystick(int index) : index(index), connected(false), axis_count(0), button_count(0) {}

bool Joystick::isConnected() const {
    return connected;
}

uint32_t Joystick::getAxisCount() const {
    return axis_count;
}

float Joystick::getAxis(uint32_t id) const {
    return id < axis_count ? axis[id] : 0.0f;
}

uint32_t Joystick::getButtonCount() const {
    return button_count;
}

boolx Joystick::getButton(uint32_t id) const {
    return id < button_count ? boolx(button[current ^ 1][id], button[current][id]) : boolx();
}

boolx Joystick::getPrimaryButton() const {
    return getButton(0);
}

boolx Joystick::getSecondaryButton() const {
    return getButton(1);
}

void Joystick::update() {
    
    // Check new status
    bool new_connected = glfwJoystickPresent(GLFW_JOYSTICK_1 + index);
    if (new_connected) {
        
        // Notify if newly connected
        if (connected)
            std::cout << "Gamepad " << index << " connected" << std::endl;
        
        // Get axes
        int count;
        float const * new_axis = glfwGetJoystickAxes(index, &count);
        axis_count = glm::max((unsigned)count, sizeof(axis) / sizeof(axis[0]));
        for (uint32_t j = 0; j < axis_count; ++j)
            axis[j] = new_axis[j];
        
        // Get buttons
        current ^= 1;
        unsigned char const * new_button = glfwGetJoystickButtons(index, &count);
        button_count = glm::max((unsigned)count, sizeof(button) / sizeof(button[0]));
        for (uint32_t j = 0; j < button_count; ++j)
            button[current][j] = new_button[j] == GLFW_PRESS;
        
    } else if (connected) {
        
        // Notify if newly disconnected
        axis_count = 0;
        button_count = 0;
        // TODO clear states?
        std::cout << "Gamepad " << index << " disconnected" << std::endl;
    }
    connected = new_connected;
}
