
#include <openvr_mingw.hpp>

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

float Controller::getAxisValue(uint32_t id) const {
    return 0.0f;
}

uint32_t Controller::getButtonCount() const {
    return 64;
}

bool Controller::isButtonDown(uint32_t id) const {
#ifndef GLOW_NO_OPENVR
    if (index >= 0 && id < 64)
        return (state.ulButtonPressed & vr::ButtonMaskFromId((vr::EVRButtonId)id)) != 0;
#endif
    return false;
}

bool Controller::isButtonPressed(uint32_t id) const {
    // TODO
    return false;
}

bool Controller::isButtonReleased(uint32_t id) const {
    // TODO
    return false;
}

Controller::Controller() : index(-1) {}
