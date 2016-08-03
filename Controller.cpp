
#include "Controller.hpp"

bool Controller::isConnected() const {
    return index >= 0;
}
    
glm::mat4 Controller::getTransform() const {
    return transform;
}

glm::vec3 Controller::getVelocity() const {
    return velocity;
}

uint32_t Controller::getAxisCount() const {
    return 0;
}

float Controller::getAxis(uint32_t id) const {
    return 0.0f;
}

uint32_t Controller::getButtonCount() const {
    return 64;
}

boolx Controller::getButton(uint32_t id) const {
#ifndef GLOW_NO_OPENVR
    if (index >= 0 && id < 64)
        return boolx(
            (state[current ^ 1].ulButtonPressed & vr::ButtonMaskFromId((vr::EVRButtonId)id)) != 0,
            (state[current].ulButtonPressed & vr::ButtonMaskFromId((vr::EVRButtonId)id)) != 0
        );
#endif
    return false;
}

boolx Controller::getPrimaryButton() const {
    return getButton(33);
}

boolx Controller::getSecondaryButton() const {
    return getButton(1);
}

Controller::Controller() : index(-1), current(0) {
    // TODO init states?
}
