
#include "Listener.hpp"
#include "Source.hpp"
#include "Sound.hpp"

#ifndef GLOW_NO_OPENAL

Listener::Listener() : device(nullptr), context(nullptr), efx(false) {}

Listener::~Listener() {
    if (device) {
        if (context) {
            
            // Release resources
            for (Binding * binding : bindings) {
                alSourceStop(binding->handle);
                alDeleteSources(1, &binding->handle);
                delete binding;
            }
            for (Sound * sound : sounds)
                delete sound;
            for (Source * source : sources)
                delete source;
            
            // Delete context
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(context);
        }
        alcCloseDevice(device);
    }
}

bool Listener::initialize() {
    
    // Only one context is allowed
    if (alcGetCurrentContext()) {
        std::cout << "Only one OpenAL context can be active at a time!" << std::endl;
        return false;
    }
    
    // Open device
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cout << "Failed to create OpenAL device" << std::endl;
        return false;
    }
    
    // Check EFX
    efx = alcIsExtensionPresent(device, ALC_EXT_EFX_NAME) == AL_TRUE;
    if (efx) {
        ALCint major, minor;
        alcGetIntegerv(device, ALC_EFX_MAJOR_VERSION, 1, &major);
        alcGetIntegerv(device, ALC_EFX_MINOR_VERSION, 1, &minor);
        std::cout << "OpenAL EFX: " << major << '.' << minor << std::endl;
    } else
        std::cout << "OpenAL EFX: <none>" << std::endl;
    
    // Create context
    ALint attribs[4] = {0}; 
    if (efx) {
        attribs[0] = ALC_MAX_AUXILIARY_SENDS;
        attribs[1] = 4;
    }
    context = alcCreateContext(device, attribs);
    if (!context || !alcMakeContextCurrent(context)) {
        std::cout << "Failed to create OpenAL context" << std::endl;
        if (context) {
            alcDestroyContext(context);
            context = nullptr;
        }
        alcCloseDevice(device);
        device = nullptr;
        return false;
    }
    
    // Print version
    std::cout << "OpenAL version: " << alGetString(AL_VERSION) << std::endl;
    std::cout << "OpenAL renderer: " << alGetString(AL_RENDERER) << std::endl;
    
    // Allocate sources
    uint32_t max_bindings = 256; // TODO define from arguments
    for (uint32_t i = 0; i < max_bindings; ++i) {
        ALuint handle;
        alGenSources(1, &handle);
        assert(handle);
        bindings.push_back(new Binding({handle, nullptr}));
    }
    return true;
}

void Listener::update() {
    
    // TODO defer some parameter updates? i.e. position and reverb parameters
    
    // Check if sources have ended
    // TODO reduce CPU overload by doing this check at a lower frequency
    ALint state;
    for (Binding * binding : bindings)
        if (binding->source) {
            alGetSourcei(binding->handle, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING)
                binding->source->stop();
        }
    
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

Sound * Listener::addSoundBuffer(Sampler & sampler) {
    Sound * sound = new Sound(&sampler);
    sounds.push_front(sound);
    return sound;
}

Sound * Listener::addSoundStream(Sampler * sampler) {
    // TODO sound stream
    return nullptr;
}

Source * Listener::addSource(Sound * sound) {
    Source * source = new Source(this);
    sources.push_front(source);
    source->iterator = sources.begin();
    source->sound = sound;
    return source;
}

#else

Listener::Listener() {}

Listener::~Listener() {}

bool Listener::initialize() {
    return false;
}

void Listener::update() {}

void Listener::setPosition(glm::vec3 const & position) {}

glm::vec3 Listener::getPosition() {
    return glm::vec3();
}

void Listener::setOrientation(glm::vec3 const & forward, glm::vec3 const & up) {}

Sound * Listener::addSoundBuffer(Sampler & sampler) {
    return nullptr;
}

Sound * Listener::addSoundStream(Sampler * sampler) {
    return nullptr;
}

Source * Listener::addSource(Sound * sound) {
    return nullptr;
}

#endif
