
#include "Camera.hpp"

Camera::Camera() : framebuffer(nullptr) {}

glm::mat4 Camera::getProjection() const {
    return projection;
}

void Camera::setProjection(glm::mat4 const & projection) {
    this->projection = projection;
}

glm::mat4 Camera::getView() const {
    return glm::inverse(getTransform());
}

Framebuffer * Camera::getFramebuffer() const {
    return framebuffer;
}

void Camera::setFramebuffer(Framebuffer * framebuffer) {
    this->framebuffer = framebuffer;
}
