
#include "Actor.hpp"

glm::vec3 Actor::getPosition() const {
    return glm::vec3(getTransform()[3]);
}
