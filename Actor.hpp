
#ifndef GLOW_ACTOR_HPP
#define GLOW_ACTOR_HPP

#include "Common.hpp"

class Actor {
public:
    
    Actor(Actor const &) = delete;
    Actor & operator=(Actor const &) = delete;
    
    virtual glm::mat4 getTransform() const = 0;
    glm::vec3 getPosition() const;
    // TODO get axes
    
    virtual glm::vec3 getVelocity() const = 0;
    // TODO angular velocity
    
protected:
    
    Actor() = default;
    virtual ~Actor() = default;
    
};

#endif
