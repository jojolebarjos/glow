
#ifndef GLOW_ACTOR_HPP
#define GLOW_ACTOR_HPP

#include "Common.hpp"

class Actor {
public:
    
    // Note on coordinate system:
    // The world is Z-up (i.e. Y is north, X is east)
    // The camera is OpenGL-based (i.e. looking toward -Z, X is right, Y is up)
    // Hence, by default (i.e. identity transform), an actor is oriented toward the ground
    
    virtual glm::mat4 getTransform() const = 0;
    glm::vec3 getPosition() const;
    glm::vec3 getForward() const;
    glm::vec3 getRight() const;
    glm::vec3 getUp() const;
    
    virtual glm::vec3 getVelocity() const = 0;
    // TODO angular velocity
    
protected:
    
    Actor() = default;
    virtual ~Actor() = default;
    
};

class AttachableActor : public Actor {
public:
    
    Actor const * getParent() const;
    void setParent(Actor const * parent);
    
    glm::mat4 getTransform() const;
    // TODO setTransform
    glm::mat4 getRelativeTransform() const;
    void setRelativeTransform(glm::mat4 const & transform);
    void setRelativeTransform(glm::vec3 const & position, glm::vec3 const & forward, glm::vec3 const & up);
    void setRelativePosition(glm::vec3 const & position);
    
    glm::vec3 getVelocity() const;
    // TODO setVelocity
    glm::vec3 getRelativeVelocity() const;
    void setRelativeVelocity(glm::vec3 const & velocity);
    
protected:
    
    AttachableActor();
    virtual ~AttachableActor() = default;
    
private:
    
    Actor const * parent;
    glm::mat4 transform;
    glm::vec3 velocity;
    
};

#endif
