
#ifndef GLOW_SCENE_HPP
#define GLOW_SCENE_HPP

#include "Common.hpp"
#include "Renderer.hpp"
#include "Listener.hpp"
#include "Source.hpp"
#include "Window.hpp"
#include "Light.hpp"
#include "Model.hpp"
#include "Physics.hpp"

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
    
    Physics physics;
    
    std::vector<Model> models;
    
};

#endif
