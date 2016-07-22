
#include "Listener.hpp"

void Listener::Sound::setPosition(glm::vec3 const & position) {
    this->position = position;
    if (source)
        alSource3f(source->handle, AL_POSITION, position.x, position.y, position.z);
}

glm::vec3 Listener::Sound::getPosition() const {
    return position;
}

void Listener::Sound::play() {
    
    // Make sure the sound is stopped
    stop();
    
    // Find available source
    for (Source * s : listener->sources)
        if (!s->sound) {
            source = s;
            break;
        }
    
    // If no source was found, ignore call
    if (!source) {
        std::cout << "warning: source limit reached" << std::endl;
        return;
    }
    
    // Define properties
    alSourcei(source->handle, AL_BUFFER, listener->buffers[buffer].handle);
    alSource3f(source->handle, AL_POSITION, position.x, position.y, position.z);
    // alSourcei(handle, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
    // alSource3f(handle, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    // alSource3f(handle, AL_DIRECTION, direction.x, direction.y, direction.z);
    // alSourcef(handle, AL_PITCH, 1); in 0.5 .. 2.0
    // TODO attentuation AL_CONE_INNER_ANGLE, AL_CONE_OUTER_ANGLE, AL_REFERENCE_DISTANCE, AL_ROLLOFF_FACTOR, AL_MAX_DISTANCE
    // TODO gain AL_GAIN, AL_MIN_GAIN, AL_MAX_GAIN, AL_CONE_OUTER_GAIN
    // TODO AL_LOOPING
    // TODO AL_SEC_OFFSET, AL_SAMPLE_OFFSET, AL_BYTE_OFFSET
    
    // Start playback
    source->sound = this;
    alSourcePlay(source->handle);
}

void Listener::Sound::stop() {
    
    // Stop and unregister source
    if (source) {
        alSourceStop(source->handle);
        source->sound = nullptr;
        source = nullptr;
    }
    
    // If this sound was released, destroy it
    if (released) {
        listener->sounds.erase(iterator);
        delete this;
        std::cout << "sound destroyed" << std::endl;
    }
}

bool Listener::Sound::isPlaying() const {
    return source;
}

void Listener::Sound::release() {
    released = true;
    if (!source)
        stop();
}

Listener::Sound::Sound(Listener * listener) : listener(listener), released(false), source(nullptr) {}

Listener::Listener() : device(nullptr), context(nullptr) {}

Listener::~Listener() {
    if (device) {
        if (context) {
            
            // Release resources
            for (Source * source : sources) {
                alSourceStop(source->handle);
                alDeleteSources(1, &source->handle);
                delete source;
            }
            for (Buffer & buffer : buffers)
                alDeleteBuffers(1, &buffer.handle);
            for (Sound * sound : sounds)
                delete sound;
            
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
    uint32_t max_sources = 256; // TODO define from arguments
    for (uint32_t i = 0; i < max_sources; ++i) {
        ALuint handle;
        alGenSources(1, &handle);
        assert(handle);
        sources.push_back(new Source({handle, nullptr}));
    }
    return true;
}

void Listener::update() {
    
    // Check if sources have ended
    ALint state;
    for (Source * source : sources)
        if (source->sound) {
            alGetSourcei(source->handle, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING)
                source->sound->stop();
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
    uint32_t index = buffers.size();
    buffers.push_back({handle});
    return index;
}

Listener::Sound * Listener::addSound(uint32_t buffer) {
    Sound * sound = new Sound(this);
    sounds.push_front(sound);
    sound->iterator = sounds.begin();
    sound->buffer = buffer;
    return sound;
}
