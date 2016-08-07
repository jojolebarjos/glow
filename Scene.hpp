
#ifndef GLOW_SCENE_HPP
#define GLOW_SCENE_HPP

#include "Common.hpp"
#include "Renderer.hpp"
#include "Listener.hpp"
#include "Source.hpp"
#include "Window.hpp"
#include "Light.hpp"
#include "Model.hpp"

class Scene {
public:
    
    Scene(Window * window);
    ~Scene();
    
    Scene(Scene const &) = delete;
    Scene & operator=(Scene const &) = delete;
    
    bool initialize();
    void update();
    
private:

    Window * window;
    int width;
    int height;
    
    float time;
    
    Camera camera;
    Renderer renderer;
    
    Listener listener;
    Sound * sound;
    Source * source;
    
    // TODO do we need to encapsulate physics in dedicated classes?
    
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
        Model model;
        
        ~Object();
        
        glm::mat4 getTransform();
    };
    std::list<Object *> objects;
    
    Object * addObject(btCollisionShape * shape, glm::vec3 const & position, float mass);
    
    Object * addCube(glm::vec3 const & position);
    
};

#endif
