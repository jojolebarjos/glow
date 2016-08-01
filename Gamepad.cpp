
#include "Gamepad.hpp"

namespace {

struct NullGamepad : Gamepad {} null;

}

Gamepad const * Gamepad::getNull() {
    return &null;
}

bool Gamepad::isConnected() const {
    return false;
}

uint32_t Gamepad::getAxisCount() const {
    return 0;
}

float Gamepad::getAxisValue(uint32_t id) const {
    return 0.0f;
}

uint32_t Gamepad::getButtonCount() const {
    return 0;
}

bool Gamepad::isButtonDown(uint32_t id) const {
    return false;
}

bool Gamepad::isButtonPressed(uint32_t id) const {
    return false;
}

bool Gamepad::isButtonReleased(uint32_t id) const {
    return false;
}
