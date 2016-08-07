
#include "Scene.hpp"
#include "Window.hpp"

Scene::Scene(Window * window) : window(window) {}

Scene::~Scene() {}

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
    renderer.loadImage("Crate.jpg");
    renderer.loadImage("Floor.jpg");
    renderer.loadImage("Metal.jpg");
    renderer.loadMesh("Cube.obj");
    renderer.pack();
    
    // Create floor
    Body * plane = new Body(&physics, new btStaticPlaneShape(btVector3(0, 0, 1), 0), {0, 0, 0}, 0);
    Model model;
    model.mesh = 0;
    model.color = 1;
    model.setParent(plane);
    models.push_back(model);
    
    // Prepare listener
    sound = nullptr;
    source = nullptr;
    if (listener.initialize()) {
        Sampler sampler;
        sampler.load("Test.wav", Sampler::LEFT);
        sound = listener.addSoundBuffer(sampler);
        source = listener.addSource(sound);
        source->setParent(window->getController(0));
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
    if (window->getKeyboard()->getPrimaryButton().isPressed() || (window->getHead() && window->getController(0)->getPrimaryButton().isPressed())) {
        glm::vec3 position(5, 0, 5);
        btBoxShape * shape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
        Body * body = new Body(&physics, shape, position, 1.0f);
        Model model;
        model.mesh = 1;
        model.color = 0;
        model.setParent(body);
        models.push_back(model);
        source->play();
    }
    
    // Simulate world
    physics.update(delta);
    
    // Update camera
    glm::vec3 position(glm::cos(time / 3) * 5.0f, glm::sin(time / 3) * 5.0f, 3.0f);
    glm::vec3 target(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 0.0f, 1.0f);
    camera.setRelativeTransform(position, target - position, up);
    
    // Define render objects
    renderer.clear();
    Light light;
    light.setRelativePosition({-1.0f, 0.0f, 3.0f});
    light.setRadius(5.0f);
    light.setColor({0.2f, 0.4f, 1.0f});
    renderer.addLight(&light);
    for (Model & model : models)
        renderer.addModel(&model);
    
    // Special objects for VR
    Light l;
    Model m;
    if (window->getHead()) {
        m.color = 0;
        m.mesh = 1;
        m.setParent(window->getController(0));
        m.setRelativeTransform(glm::mat4(0.1, 0, 0, 0, 0, 0.1, 0, 0, 0, 0, 0.1, 0, 0, 0, 0, 1));
        renderer.addModel(&m);
        l.setParent(window->getController(1));
        l.setRadius(6.0f);
        l.setColor({1.0f, 0.7f, 0.2f});
        renderer.addLight(&l);
    }
    
    // Draw everything
    renderer.prepare();
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
