
#include "Actor.hpp"

glm::vec3 Actor::getPosition() const {
    return glm::vec3(getTransform()[3]);
}

Actor const * AttachableActor::getParent() const {
    return parent;
}

void AttachableActor::setParent(Actor const * parent) {
    this->parent = parent;
}

glm::mat4 AttachableActor::getTransform() const {
    if (parent)
        return parent->getTransform() * transform;
    return transform;
}

glm::mat4 AttachableActor::getRelativeTransform() const {
    return transform;
}

void AttachableActor::setRelativeTransform(glm::mat4 const & transform) {
    this->transform = transform;
}

void AttachableActor::setRelativeTransform(glm::vec3 const & position, glm::vec3 const & forward, glm::vec3 const & up) {
    this->transform = glm::inverse(glm::lookAt(position, position + forward, up));
}

glm::vec3 AttachableActor::getVelocity() const {
    if (parent)
        return parent->getVelocity() + glm::vec3(parent->getTransform() * glm::vec4(velocity, 0.0f));
    return velocity;
}

glm::vec3 AttachableActor::getRelativeVelocity() const {
    return velocity;
}

void AttachableActor::setRelativeVelocity(glm::vec3 const & velocity) {
    this->velocity = velocity;
}

AttachableActor::AttachableActor() : parent(nullptr) {}
