
#include "Camera.hpp"

Camera::Camera() : framebuffer(nullptr) {}

glm::mat4 Camera::getTransform() const {
    return transform;
}

void Camera::setTransform(glm::mat4 const & transform) {
    this->transform = transform;
}

glm::vec3 Camera::getVelocity() const {
    return velocity;
}

void Camera::setVelocity(glm::vec3 const & velocity) {
    this->velocity = velocity;
}

glm::mat4 Camera::getProjection() const {
    return projection;
}

void Camera::setProjection(glm::mat4 const & projection) {
    this->projection = projection;
}

glm::mat4 Camera::getView() const {
    return view;
}

void Camera::setView(glm::mat4 const & view) {
    this->view = view;
}

Framebuffer * Camera::getFramebuffer() const {
    return framebuffer;
}

void Camera::setFramebuffer(Framebuffer * framebuffer) {
    this->framebuffer = framebuffer;
}
