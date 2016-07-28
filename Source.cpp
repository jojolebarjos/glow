
#include "Source.hpp"

#ifdef GLOW_OPENAL

void Source::setPosition(glm::vec3 const & position) {
    this->position = position;
    if (binding)
        alSource3f(binding->handle, AL_POSITION, position.x, position.y, position.z);
}

glm::vec3 Source::getPosition() const {
    return position;
}

void Source::play() {
    
    // Make sure the sound is stopped
    stop();
    
    // Find available source
    for (Listener::Binding * b : listener->bindings)
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
    alSourcei(binding->handle, AL_BUFFER, sound->handle);
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

void Source::stop() {
    
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

bool Source::isPlaying() const {
    return binding;
}

void Source::release() {
    released = true;
    if (!binding)
        stop();
}

Source::Source(Listener * listener) : listener(listener), released(false), binding(nullptr) {}

#else

void Source::setPosition(glm::vec3 const & position) {}

glm::vec3 Source::getPosition() const {
    return glm::vec3();
}

void Source::play() {}

void Source::stop() {}

bool Source::isPlaying() const {
    return false;
}

void Source::release() {}

Source::Source(Listener * listener) {}

#endif
