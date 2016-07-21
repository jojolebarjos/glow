
#include "Listener.hpp"

Listener::Listener() : device(nullptr), context(nullptr) {}

Listener::~Listener() {
    if (device) {
        if (context) {
            
            // Release resources
            for (Source & source : sources) {
                // TODO should we stop the sources?
                alDeleteSources(1, &source.handle);
            }
            for (Sound & sound : sounds)
                alDeleteBuffers(1, &sound.handle);
            
            // Delete context
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
    
    // Allocate sources
    uint32_t max_sources = 1024; // TODO define from arguments
    for (uint32_t i = 0; i < max_sources; ++i) {
        ALuint handle;
        alGenSources(1, &handle);
        assert(handle);
        sources.push_back({handle});
    }
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

uint32_t Listener::addSound(Sampler & sampler) {
    // TODO on error, return -1?
    
    // Create buffer
    ALuint handle;
    alGenBuffers(1, &handle);
    assert(handle);
    
    // Read bytes
    uint32_t length = sampler.getBytes();
    char * bytes = new char[length];
    sampler.rewind();
    uint32_t count = sampler.read(bytes, sampler.getSize());
    std::cout << count <<'/'<<sampler.getSize()<< std::endl;
    
    // Store data
    alBufferData(handle, sampler.getFormat(), bytes, length, sampler.getFrequency());
    
    // Save sound
    uint32_t index = sounds.size();
    sounds.push_back({handle});
    return index;
}

int32_t Listener::playSound(uint32_t sound, glm::vec3 const & position) {
    
    // Find available source
    // TODO improve this strategy:
    // - a source might become available afterwards, so do not discard it if none currently available
    // - use a priority system to sort virtual sources
    int32_t index = -1;
    for (uint32_t i = 0; i < sources.size(); ++i) {
        ALint state;
        alGetSourcei(sources[i].handle, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING) {
            index = i;
            break;
        }
    }
    if (index < 0)
        return -1;
    
    // Setup source parameters
    // alSourcei(handle, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
    // alSource3f(handle, AL_POSITION, position.x, position.y, position.z);
    // alSource3f(handle, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    // alSource3f(handle, AL_DIRECTION, direction.x, direction.y, direction.z);
    // alSourcef(handle, AL_PITCH, 1); in 0.5 .. 2.0
    // alSourcei(handle, AL_BUFFER, buffhandle);
    // TODO attentuation AL_CONE_INNER_ANGLE, AL_CONE_OUTER_ANGLE, AL_REFERENCE_DISTANCE, AL_ROLLOFF_FACTOR, AL_MAX_DISTANCE
    // TODO gain AL_GAIN, AL_MIN_GAIN, AL_MAX_GAIN, AL_CONE_OUTER_GAIN
    // TODO AL_LOOPING
    ALuint handle = sources[index].handle;
    alSource3f(handle, AL_POSITION, position.x, position.y, position.z);
    alSourcei(handle, AL_BUFFER, sounds[sound].handle);
    
    // Start playback
    alSourcePlay(handle);
    // TODO later this id will be used to modify the sound afterward (change position, stop...)
    // TODO AL_SEC_OFFSET, AL_SAMPLE_OFFSET, AL_BYTE_OFFSET
    return index;
}
