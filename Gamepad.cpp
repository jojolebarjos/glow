
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

float Gamepad::getAxis(uint32_t id) const {
    return 0.0f;
}

uint32_t Gamepad::getButtonCount() const {
    return 0;
}

boolx Gamepad::getButton(uint32_t id) const {
    return false;
}

boolx Gamepad::getAnyButton() const {
    boolx result;
    uint32_t count = getButtonCount();
    for (uint32_t i = 0; i < count; ++i) {
        boolx b = getButton(i);
        result.previous |= b.previous;
        result.current |= b.current;
    }
    return result;
}

boolx Gamepad::getPrimaryButton() const {
    return false;
}

boolx Gamepad::getSecondaryButton() const {
    return false;
}
