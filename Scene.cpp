
#include "Scene.hpp"

Scene::Scene(GLFWwindow * window) : window(window), renderer(window), world(nullptr), solver(nullptr), dispatcher(nullptr), configuration(nullptr), broadphase(nullptr) {}

Scene::~Scene() {
    while (!objects.empty())
        delete objects.front();
    delete world;
    delete solver;
    delete dispatcher;
    delete configuration;
    delete broadphase;
}

bool Scene::initialize() {
    // TODO handle errors
    
    // Get screen size
    glfwGetFramebufferSize(window, &width, &height);
    
    // Initialize time
    time = 0;
    
    // Prepare renderer
    renderer.initialize();
    renderer.loadImage("Test.bmp");
    renderer.loadMesh("Cube.obj");
    renderer.loadMesh("Square.obj");
    renderer.pack();
    
    // Create physics
    broadphase = new btDbvtBroadphase();
    configuration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(configuration);
    solver = new btSequentialImpulseConstraintSolver();
    world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, configuration);
    world->setGravity({0, 0, -10});
    
    // Create floor
    Object * plane = addObject(new btStaticPlaneShape(btVector3(0, 0, 1), 0), {0, 0, 0}, 0);
    plane->mesh.mesh = 1;
    plane->mesh.color = 0;
    
    // Prepare listener
    listener.initialize();
    Sampler sampler;
    sampler.load("Test.wav", Sampler::LEFT);
    listener.addSoundBuffer(sampler);
    
    return true;
}

void Scene::update() {
    
    // Update time
    float now = glfwGetTime();
    float delta = now - time;
    time = now;
    
    // Add cube if requested
    static bool just = false;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !just) {
        addCube({5, 0, 5});
        Listener::Sound * sound = listener.addSound(0);
        sound->setPosition({5, 0, 0});
        sound->play();
        sound->release();
        just = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        just = false;
    
    // Simulate world
    world->stepSimulation(delta, 10);
    
    // Update camera
    glm::vec3 position(glm::cos(time / 3) * 5.0f, glm::sin(time / 3) * 5.0f, 3.0f);
    glm::vec3 target(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 0.0f, 1.0f);
    glm::mat4 view = glm::lookAt(position, target, up);
    renderer.setView(view);
    
    // Define render objects
    renderer.clear();
    renderer.addLight({{1, 1, 2}, 5, {1, 0.8, 0.2}});
    renderer.addLight({{-1, 0, 3}, 5, {0.2, 0.4, 1}});
    for (Object * object : objects) {
        object->mesh.transform = object->getTransform();
        renderer.addMesh(object->mesh);
    }
    
    // Draw everything
    renderer.render();
    
    // Update audio
    listener.setPosition(position);
    listener.setOrientation(target - position, up);
    listener.update();
    
}

Scene::Object::~Object() {
    scene->world->removeRigidBody(body);
    delete shape;
    delete body->getMotionState();
    delete body;
    scene->objects.erase(iterator);
}

glm::mat4 Scene::Object::getTransform() {
    btTransform transform;
    body->getMotionState()->getWorldTransform(transform);
    glm::mat4 matrix;
    transform.getOpenGLMatrix(glm::value_ptr(matrix));
    return matrix;
}

Scene::Object * Scene::addObject(btCollisionShape * shape, glm::vec3 const & position, float mass) {
    Object * result = new Object();
    result->scene = this;
    result->shape = shape;
    btMotionState * state = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(position.x, position.y, position.z)));
    btVector3 inertia(0, 0, 0);
    shape->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo ci(mass, state, shape, inertia);
    result->body = new btRigidBody(ci);
    world->addRigidBody(result->body);
    objects.push_front(result);
    result->iterator = objects.begin();
    return result;
}

Scene::Object * Scene::addCube(glm::vec3 const & position) {
    btBoxShape * shape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
    Object * result = addObject(shape, position, 1.0f);
    result->mesh.mesh = 0;
    result->mesh.color = 0;
    return result;
}
