
#include "Listener.hpp"

Listener::Listener() : device(nullptr), context(nullptr) {}

Listener::~Listener() {
    if (device) {
        if (context) {
            alcMakeContextCurrent(NULL);
            alcDestroyContext(context);
        }
        alcCloseDevice(device);
    }
}

bool Listener::initialize() {
    // TODO only allow one context/listener
    // TODO proper error/resource management
    
    // Open device
    device = alcOpenDevice(NULL);
    if (!device) {
        std::cout << "Failed to create OpenAL device" << std::endl;
        return false;
    }
    
    // Create context
    context = alcCreateContext(device, NULL);
    if (!context || !alcMakeContextCurrent(context)) {
        std::cout << "Failed to create OpenAL context" << std::endl;
        return false;
    }
    
    // Print infos
    std::cout << "OpenAL version: " << alGetString(AL_VERSION) << std::endl;
    //std::cout << "OpenAL vendor: " << alGetString(AL_VENDOR) << std::endl;
    std::cout << "OpenAL renderer: " << alGetString(AL_RENDERER) << std::endl;
    //std::cout << "OpenAL extensions: " << alGetString(AL_EXTENSIONS) << std::endl;
    //std::cout << "OpenAL context: " << alcGetString(device, ALC_DEVICE_SPECIFIER) << std::endl;
    
    // Ready
    while (alGetError() != AL_NO_ERROR);
    return true;
}

void Listener::update() {
    // TODO manage virtual source priorities
    // TODO update streamed sound
}

void Listener::setPosition(glm::vec3 const & position) {
    alListener3f(AL_POSITION, position.x, position.y, position.z);
}

glm::vec3 Listener::getPosition() {
    glm::vec3 position;
    alGetListener3f(AL_POSITION, &position.x, &position.y, &position.z);
    return position;
}

void Listener::setOrientation(glm::vec3 const & forward, glm::vec3 const & up) {
    ALfloat orientation[] = {forward.x, forward.y, forward.z, up.x, up.y, up.z};
    alListenerfv(AL_ORIENTATION, orientation);
}
