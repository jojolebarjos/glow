
#ifndef GLOW_SCENE_HPP
#define GLOW_SCENE_HPP

#include "Common.hpp"
#include "Renderer.hpp"

class Scene {
public:
    
    Scene(GLFWwindow * window);
    ~Scene();
    
    Scene(Scene const &) = delete;
    Scene & operator=(Scene const &) = delete;
    
    bool initialize();
    void update();
    
private:

    GLFWwindow * window;
    int width;
    int height;
    
    float time;
    
    Renderer renderer;
    
    btDynamicsWorld * world;
    btConstraintSolver * solver;
    btCollisionDispatcher * dispatcher;
    btCollisionConfiguration * configuration;
    btBroadphaseInterface * broadphase;
    
    struct Object {
        // TODO dramatically improve the object model!
        Scene * scene;
        std::list<Object *>::iterator iterator;
        btCollisionShape * shape;
        btRigidBody * body;
        Renderer::MeshInfo mesh;
        
        ~Object();
        
        glm::mat4 getTransform();
    };
    std::list<Object *> objects;
    
    Object * addObject(btCollisionShape * shape, glm::vec3 const & position, float mass);
    
    Object * addCube(glm::vec3 const & position);
    
};

#endif
