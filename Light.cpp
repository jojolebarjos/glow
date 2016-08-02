
#include "Light.hpp"

float Light::getRadius() const {
    return radius;
}

void Light::setRadius(float radius) {
    this->radius = radius;
}

glm::vec3 Light::getColor() const {
    return color;
}

void Light::setColor(glm::vec3 const & color) {
    this->color = color;
}
