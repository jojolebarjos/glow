
#include "Physics.hpp"

Physics::Physics() {
    
    // Create Bullet components
    broadphase = new btDbvtBroadphase();
    configuration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(configuration);
    solver = new btSequentialImpulseConstraintSolver();
    world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, configuration);
    
    // Set default properties
    world->setGravity({0, 0, -10});
}

Physics::~Physics() {
    // TODO not sure if the world should have authority on objects...
    while (!bodies.empty())
        delete bodies.front();
    delete world;
    delete solver;
    delete dispatcher;
    delete configuration;
    delete broadphase;
}

void Physics::update(float delta) {
    world->stepSimulation(delta, 10);
}
