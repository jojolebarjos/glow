
#include "Scene.hpp"
#include "Window.hpp"

Scene::Scene(Window * window) : window(window), renderer(window), world(nullptr), solver(nullptr), dispatcher(nullptr), configuration(nullptr), broadphase(nullptr) {}

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
    
    // Prepare camera
    camera.setProjection(glm::perspective(PI / 3.0f, (float)window->getWidth() / (float)window->getHeight(), 0.1f, 1000.0f));
    
    // Prepare renderer
    uint32_t width, height;
    if (window->getHead()) {
        width = window->getHead()->getWidth();
        height = window->getHead()->getHeight();
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
    sound = nullptr;
    source = nullptr;
    if (listener.initialize()) {
        Sampler sampler;
        sampler.load("Test.wav", Sampler::LEFT);
        sound = listener.addSoundBuffer(sampler);
        source = listener.addSource(sound);
    }
    if (window->getHead())
        listener.setParent(window->getHead());
    else
        listener.setParent(&camera);
    
    return true;
}

void Scene::update() {
    
    // Update time
    float now = glfwGetTime();
    float delta = now - time;
    time = now;
    
    // Add cube if requested
    static bool just = false;
    if (window->getKeyboard()->isButtonPressed(GLFW_KEY_SPACE) || (!just && window->getHead() && window->getController(0)->isButtonDown(33))) {
        addCube({5, 0, 5});
        just = true;
        source->play();
    }
    if (window->getHead() && !window->getController(0)->isButtonDown(33))
        just = false;
    
    // Simulate world
    world->stepSimulation(delta, 10);
    
    // Update camera
    glm::vec3 position(glm::cos(time / 3) * 5.0f, glm::sin(time / 3) * 5.0f, 3.0f);
    glm::vec3 target(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 0.0f, 1.0f);
    camera.setRelativeTransform(position, target - position, up);
    
    // Define render objects
    renderer.clear();
    //renderer.addLight({{1, 1, 2}, 5, {1, 0.8, 0.2}});
    renderer.addLight({{-1, 0, 3}, 5, {0.2, 0.4, 1}});
    for (Object * object : objects) {
        object->mesh.transform = object->getTransform();
        renderer.addMesh(object->mesh);
    }
    
    // Special objects for VR
    if (window->getHead()) {
        Renderer::MeshInfo m;
        m.color = 0;
        m.mesh = 1;
        m.transform = window->getController(0)->getTransform() * glm::mat4(0.1, 0, 0, 0, 0, 0.1, 0, 0, 0, 0, 0.1, 0, 0, 0, 0, 1);
        renderer.addMesh(m);
        Renderer::LightInfo l;
        l.color = glm::vec3(1.0, 0.7, 0.2);
        l.position = window->getController(1)->getPosition();
        l.radius = 6;
        renderer.addLight(l);
        source->setPosition(window->getController(0)->getPosition());
        source->setVelocity(window->getController(0)->getVelocity());
    }
    
    // Draw everything
    if (window->getHead()) {
        glViewport(0, 0, window->getHead()->getWidth(), window->getHead()->getHeight());
        for (unsigned int i = 0; i < 2; ++i)
            renderer.render(window->getHead()->getEye(i));
    } else {
        glViewport(0, 0, window->getWidth(), window->getHeight());
        renderer.render(&camera);
    }
    
    // Update audio
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
