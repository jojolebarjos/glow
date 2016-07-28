
#include "Scene.hpp"
#include "Window.hpp"

Scene::Scene(Window * window) : window(window), world(nullptr), solver(nullptr), dispatcher(nullptr), configuration(nullptr), broadphase(nullptr) {}

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
    
    // Initialize time
    time = 0;
    
    // Prepare renderer
    uint32_t width, height;
    if (window->hasStereoscopy()) {
        width = window->getEyeWidth();
        height = window->getEyeHeight();
    } else {
        width = window->getWidth();
        height = window->getHeight();
    }
    renderer.initialize(width, height);
    renderer.loadImage("Test.bmp");
    renderer.loadMesh("Cube.obj");
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
    plane->mesh.mesh = 0;
    plane->mesh.color = 0;
    
    // Prepare listener
    listener.initialize();
    Sampler sampler;
    sampler.load("Test.wav", Sampler::LEFT);
    sound = listener.addSoundBuffer(sampler);
    
    return true;
}

void Scene::update() {
    
    // Update time
    float now = glfwGetTime();
    float delta = now - time;
    time = now;
    
    // Add cube if requested
    static bool just = false;
    if (glfwGetKey(window->getHandle(), GLFW_KEY_SPACE) == GLFW_PRESS && !just) {
        addCube({5, 0, 5});
        Source * source = listener.addSource(sound);
        source->setPosition({5, 0, 0});
        source->play();
        source->release();
        just = true;
    }
    if (glfwGetKey(window->getHandle(), GLFW_KEY_SPACE) == GLFW_RELEASE)
        just = false;
    
    // Simulate world
    world->stepSimulation(delta, 10);
    
    // Update camera
    glm::vec3 position(glm::cos(time / 3) * 5.0f, glm::sin(time / 3) * 5.0f, 3.0f);
    glm::vec3 target(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 0.0f, 1.0f);
    glm::mat4 view = glm::lookAt(position, target, up);
    
    // Define render objects
    renderer.clear();
    renderer.addLight({{1, 1, 2}, 5, {1, 0.8, 0.2}});
    renderer.addLight({{-1, 0, 3}, 5, {0.2, 0.4, 1}});
    for (Object * object : objects) {
        object->mesh.transform = object->getTransform();
        renderer.addMesh(object->mesh);
    }
    
    // Draw everything
    if (window->hasStereoscopy()) {
        for (unsigned int i = 0; i < 2; ++i)
            renderer.render(window->getEyeFramebuffer(i)->getHandle(), window->getEyeProjection(i), window->getEyeView(i));
    } else {
        glm::mat4 projection = glm::perspective(PI / 3.0f, (float)window->getWidth() / (float)window->getHeight(), 0.1f, 1000.0f);
        renderer.render(0, projection, view);
    }
    
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
    result->mesh.mesh = 1;
    result->mesh.color = 0;
    return result;
}
