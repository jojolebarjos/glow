
#include "Listener.hpp"

void Listener::Source::setPosition(glm::vec3 const & position) {
    this->position = position;
    if (binding)
        alSource3f(binding->handle, AL_POSITION, position.x, position.y, position.z);
}

glm::vec3 Listener::Source::getPosition() const {
    return position;
}

void Listener::Source::play() {
    
    // Make sure the sound is stopped
    stop();
    
    // Find available source
    for (Binding * b : listener->bindings)
        if (!b->source) {
            binding = b;
            break;
        }
    
    // If no source was found, ignore call
    if (!binding) {
        // TODO discard another sound with smaller priority
        std::cout << "warning: source limit reached" << std::endl;
        return;
    }
    
    // Define properties
    alSourcei(binding->handle, AL_BUFFER, listener->sounds[sound].handle);
    alSource3f(binding->handle, AL_POSITION, position.x, position.y, position.z);
    // alSourcei(handle, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
    // alSource3f(handle, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    // alSource3f(handle, AL_DIRECTION, direction.x, direction.y, direction.z);
    // alSourcef(handle, AL_PITCH, 1); in 0.5 .. 2.0
    // TODO attentuation AL_CONE_INNER_ANGLE, AL_CONE_OUTER_ANGLE, AL_REFERENCE_DISTANCE, AL_ROLLOFF_FACTOR, AL_MAX_DISTANCE
    // TODO gain AL_GAIN, AL_MIN_GAIN, AL_MAX_GAIN, AL_CONE_OUTER_GAIN
    // TODO AL_LOOPING
    // TODO AL_SEC_OFFSET, AL_SAMPLE_OFFSET, AL_BYTE_OFFSET
    
    // Start playback
    binding->source = this;
    alSourcePlay(binding->handle);
}

void Listener::Source::stop() {
    
    // Stop and unregister source
    if (binding) {
        alSourceStop(binding->handle);
        binding->source = nullptr;
        binding = nullptr;
    }
    
    // If this sound was released, destroy it
    if (released) {
        listener->sources.erase(iterator);
        delete this;
    }
}

bool Listener::Source::isPlaying() const {
    return binding;
}

void Listener::Source::release() {
    released = true;
    if (!binding)
        stop();
}

Listener::Source::Source(Listener * listener) : listener(listener), released(false), binding(nullptr) {}

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
            for (Sound & sound : sounds)
                alDeleteBuffers(1, &sound.handle);
            for (Source * source : sources)
                delete source;
            
            // Delete context
            alcMakeContextCurrent(NULL);
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
    device = alcOpenDevice(NULL);
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

uint32_t Listener::addSoundBuffer(Sampler & sampler) {
    
    // Create buffer
    ALuint handle;
    alGenBuffers(1, &handle);
    assert(handle);
    
    // Read bytes
    uint32_t length = sampler.getBytes();
    char * bytes = new char[length];
    sampler.rewind();
    sampler.read(bytes, sampler.getSize());
    
    // Store data
    alBufferData(handle, sampler.getFormat(), bytes, length, sampler.getFrequency());
    
    // Save sound
    uint32_t index = sounds.size();
    sounds.push_back({handle});
    return index;
}

Listener::Source * Listener::addSource(uint32_t sound) {
    Source * source = new Source(this);
    sources.push_front(source);
    source->iterator = sources.begin();
    source->sound = sound;
    return source;
}
