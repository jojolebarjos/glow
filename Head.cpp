
#include "Head.hpp"

glm::mat4 Head::getTransform() const {
    return transform;
}

glm::vec3 Head::getVelocity() const {
    return velocity;
}

uint32_t Head::getWidth() const {
    return width;
}

uint32_t Head::getHeight() const {
    return height;
}

Camera const * Head::getEye(uint32_t index) const {
    return index == 0 ? &left : index == 1 ? &right : nullptr;
}

Camera const * Head::getEyeLeft() const {
    return &left;
}

Camera const * Head::getEyeRight() const {
    return &right;
}
