
#ifndef GLOW_PHYSICS_HPP
#define GLOW_PHYSICS_HPP

#include "Common.hpp"
#include "Body.hpp"

class Physics {
    friend class Body;
public:
    
    Physics();
    ~Physics();
    
    Physics(Physics const &) = delete;
    Physics & operator=(Physics const &) = delete;
    
    void update(float delta);
    
    // TODO get/set world properties (gravity...)
    
    // TODO queries (trace, is free...)
    
private:

    btDynamicsWorld * world;
    btConstraintSolver * solver;
    btCollisionDispatcher * dispatcher;
    btCollisionConfiguration * configuration;
    btBroadphaseInterface * broadphase;
    
    // TODO is it really useful to keep this list?
    std::list<Body *> bodies;
    
};

#endif
