
#include "Listener.hpp"
#include "Source.hpp"
#include "Sound.hpp"

Listener::Listener()
#ifndef GLOW_NO_OPENAL
: device(nullptr), context(nullptr), efx(false)
#endif
{}

Listener::~Listener() {
#ifndef GLOW_NO_OPENAL
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
#endif
}

bool Listener::initialize() {
#ifndef GLOW_NO_OPENAL
    
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
    
#else
    std::cout << "OpenAL is not available" << std::endl;
    return false;
#endif
}

void Listener::update() {
#ifndef GLOW_NO_OPENAL
    
    // TODO defer some parameter updates? i.e. position and reverb parameters
    // TODO reduce CPU overload by doing these checks at a lower frequency
    
    // Check if sources have ended
    ALint state;
    for (Binding * binding : bindings)
        if (binding->source) {
            alGetSourcei(binding->handle, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING)
                binding->source->stop();
        }
    
    // Update streamed sounds
    for (Sound * sound : sounds)
        sound->update();
    
#endif
}

void Listener::setPosition(glm::vec3 const & position) {
#ifndef GLOW_NO_OPENAL
    alListener3f(AL_POSITION, position.x, position.y, position.z);
#endif
}

glm::vec3 Listener::getPosition() {
    glm::vec3 position;
#ifndef GLOW_NO_OPENAL
    alGetListener3f(AL_POSITION, &position.x, &position.y, &position.z);
#endif
    return position;
}

void Listener::setOrientation(glm::vec3 const & forward, glm::vec3 const & up) {
#ifndef GLOW_NO_OPENAL
    ALfloat orientation[] = {forward.x, forward.y, forward.z, up.x, up.y, up.z};
    alListenerfv(AL_ORIENTATION, orientation);
#endif
}

void Listener::getOrientation(glm::vec3 & forward, glm::vec3 & up) {
#ifndef GLOW_NO_OPENAL
    ALfloat orientation[6];
    alListenerfv(AL_ORIENTATION, orientation);
    forward.x = orientation[0];
    forward.y = orientation[1];
    forward.z = orientation[2];
    up.x = orientation[3];
    up.y = orientation[4];
    up.z = orientation[5];
#else
    forward = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
#endif
}

void Listener::setTransform(glm::mat4 const & transform) {
    setPosition(glm::vec3(transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
    setOrientation(
        glm::vec3(transform * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)),
        glm::vec3(transform * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f))
    );
}

void Listener::setVelocity(glm::vec3 const & velocity) {
#ifndef GLOW_NO_OPENAL
    alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
#endif
}

glm::vec3 Listener::getVelocity() const {
    glm::vec3 velocity;
#ifndef GLOW_NO_OPENAL
    alGetListener3f(AL_VELOCITY, &velocity.x, &velocity.y, &velocity.z);
#endif
    return velocity;
}

Sound * Listener::addSoundBuffer(Sampler & sampler) {
#ifndef GLOW_NO_OPENAL
    if (device) {
        Sound * sound = new Sound(&sampler, false);
        sounds.push_front(sound);
        return sound;
    }
#endif
    return nullptr;
}

Sound * Listener::addSoundStream(Sampler * sampler) {
#ifndef GLOW_NO_OPENAL
    if (device) {
        Sound * sound = new Sound(sampler, true);
        sounds.push_front(sound);
        return sound;
    }
#endif
    return nullptr;
}

Source * Listener::addSource(Sound * sound) {
#ifndef GLOW_NO_OPENAL
    if (device && sound) {
        Source * source = new Source(this);
        sources.push_front(source);
        source->iterator = sources.begin();
        source->sound = sound;
        return source;
    }
#endif
    return nullptr;
}
