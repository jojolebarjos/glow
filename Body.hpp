
#ifndef GLOW_BODY_HPP
#define GLOW_BODY_HPP

#include "Common.hpp"
#include "Actor.hpp"

class Physics;

class Body : public Actor {
    friend class Physics;
public:
    
    // TODO do not define these properties in constructor
    Body(Physics * physics, btCollisionShape * shape, glm::vec3 const & position, float mass);
    ~Body();
    
    Body(Body const &) = delete;
    Body & operator=(Body const &) = delete;
    
    glm::mat4 getTransform() const;
    glm::vec3 getVelocity() const;
    
    // TODO can be attached to another actor (i.e. authoritative fixed object)
    // TODO also allow "soft" attach, probably using spring constraints?
    
    // TODO automate copy
    // TODO set physics universe (i.e. can be detached from a world)?
    
    // TODO set physical properties (mass, inertia, friction, bounciness...)
    
    // TODO apply force, impulse, torque...
    
    // TODO constraints
    
private:
    
    Physics * physics;
    std::list<Body *>::iterator iterator;
    
    btCollisionShape * shape;
    btRigidBody * body;

};

#endif
