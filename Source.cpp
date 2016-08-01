
#include "Source.hpp"

void Source::setLooping(bool looping) {
    
    // Update flag
    if (looping == this->looping)
        return;
    this->looping = looping;
    
    // If the source is playing, need to notify OpenAL
#ifndef GLOW_NO_OPENAL
    if (binding) {
        
        // Buffered source loop can be managed by OpenAL
        if (!sound->sampler) {
            alSourcei(binding->handle, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
            return;
        }
        
        // For streamed source, force update
        if (looping)
            sound->queue();
        // Note: disabling looping does not unqueue unprocessed buffers
    }
#endif
}

bool Source::isLooping() const {
    return looping;
}

void Source::setPosition(glm::vec3 const & position) {
    
    // Update property
    if (position == this->position)
        return;
    this->position = position;
    
    // If the source is playing, need to notify OpenAL
#ifndef GLOW_NO_OPENAL
    if (binding)
        alSource3f(binding->handle, AL_POSITION, position.x, position.y, position.z);
#endif
}

glm::vec3 Source::getPosition() const {
    return position;
}

void Source::setVelocity(glm::vec3 const & velocity) {
    
    // Update property
    if (velocity == this->velocity)
        return;
    this->velocity = velocity;
    
    // If the source is playing, need to notify OpenAL
#ifndef GLOW_NO_OPENAL
    if (binding)
        alSource3f(binding->handle, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
#endif
}

glm::vec3 Source::getVelocity() const {
    return velocity;
}

void Source::play() {
#ifndef GLOW_NO_OPENAL
    
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
    
    // Attach sound
    if (!sound->attach(this)) {
        binding = nullptr;
        return;
    }
    
    // Enable loop for buffered sounds (streamed sound lopp will be handled in sound update)
    if (!sound->sampler)
        alSourcei(binding->handle, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
    
    // Define properties
    alSource3f(binding->handle, AL_POSITION, position.x, position.y, position.z);
    alSource3f(binding->handle, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    // alSourcei(handle, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
    // alSource3f(handle, AL_DIRECTION, direction.x, direction.y, direction.z);
    // alSourcef(handle, AL_PITCH, 1); in 0.5 .. 2.0
    // TODO attentuation AL_CONE_INNER_ANGLE, AL_CONE_OUTER_ANGLE, AL_REFERENCE_DISTANCE, AL_ROLLOFF_FACTOR, AL_MAX_DISTANCE
    // TODO gain AL_GAIN, AL_MIN_GAIN, AL_MAX_GAIN, AL_CONE_OUTER_GAIN
    // TODO AL_SEC_OFFSET, AL_SAMPLE_OFFSET, AL_BYTE_OFFSET
    
    // Start playback
    binding->source = this;
    alSourcePlay(binding->handle);
    
#endif
}

void Source::pause() {
#ifndef GLOW_NO_OPENAL
    if (isPlaying() && !paused) {
        alSourcePause(binding->handle);
        paused = true;
    }
#endif
}

void Source::resume() {
#ifndef GLOW_NO_OPENAL
    if (paused) {
        alSourcePlay(binding->handle);
        paused = false;
    }
#endif
}


void Source::stop() {
    
#ifndef GLOW_NO_OPENAL
    // Stop and unregister source
    if (binding) {
        alSourceStop(binding->handle);
        sound->detach();
        binding->source = nullptr;
        binding = nullptr;
        paused = false;
    }
#endif
    
    // If this sound was released, destroy it
    if (released) {
        listener->sources.erase(iterator);
        delete this;
    }
}

bool Source::isPlaying() const {
#ifndef GLOW_NO_OPENAL
    return binding;
#else
    return false;
#endif
}

bool Source::isPaused() const {
#ifndef GLOW_NO_OPENAL
    return paused;
#else
    return false;
#endif
}

void Source::release() {
    released = true;
    if (!isPlaying())
        stop();
}

Source::Source(Listener * listener) : listener(listener), released(false),
#ifndef GLOW_NO_OPENAL
    binding(nullptr), paused(false),
#endif
    looping(false) {}
