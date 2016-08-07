
#include "Body.hpp"
#include "Physics.hpp"

Body::Body(Physics * physics, btCollisionShape * shape, glm::vec3 const & position, float mass) {
    
    // Create physical body
    btMotionState * state = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(position.x, position.y, position.z)));
    btVector3 inertia(0, 0, 0);
    shape->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo ci(mass, state, shape, inertia);
    this->shape = shape;
    body = new btRigidBody(ci);
    
    // Register to world
    this->physics = physics;
    physics->world->addRigidBody(body);
    physics->bodies.push_front(this);
    iterator = physics->bodies.begin();
}

Body::~Body() {
    
    // Unregister from world
    physics->world->removeRigidBody(body);
    physics->bodies.erase(iterator);
    
    // Destroy physical body
    delete shape;
    delete body->getMotionState();
    delete body;
}

glm::mat4 Body::getTransform() const {
    btTransform transform;
    body->getMotionState()->getWorldTransform(transform);
    glm::mat4 matrix;
    transform.getOpenGLMatrix(glm::value_ptr(matrix));
    return matrix;
}

glm::vec3 Body::getVelocity() const {
    btVector3 velocity = body->getLinearVelocity();
    return {velocity.getX(), velocity.getY(), velocity.getZ()};
}
