
#include "Sound.hpp"
#include "Source.hpp"

bool Sound::isStream() const {
    return sampler;
}

#ifndef GLOW_NO_OPENAL

Sound::Sound(Sampler * sampler, bool stream) {
    
    // Make sure the sampler is at beginning
    sampler->rewind();
    source = nullptr;
    
    // Streamed sound only load samples when bound to a source
    if (stream) {
        alGenBuffers(5, cycle);
        for (int i = 0; i < 5; ++i) {
            assert(cycle[i]);
            available.push_back(cycle[i]);
        }
        this->sampler = sampler;
        return;
    }
    
    // Buffered sound immediately load one large buffer for the whole sampler
    alGenBuffers(1, &handle);
    assert(handle);
    this->sampler = sampler;
    fill(handle, sampler->getSize(), false);
    this->sampler = nullptr;
}

Sound::~Sound() {
    // Note: sounds are released last, so do not need to worry about being in use
    
    // Release resources
    if (sampler) {
        alDeleteBuffers(5, cycle);
        delete sampler;
    } else
        alDeleteBuffers(1, &handle);
}

uint32_t Sound::fill(ALuint buffer, uint32_t samples, bool loop) {
    char * bytes = new char[samples * sampler->getBytesPerSample()];
    uint32_t read = sampler->read(bytes, samples);
    if (read < samples && loop) {
        sampler->rewind();
        read += sampler->read(bytes + read * sampler->getBytesPerSample(), samples - read);
    }
    if (read > 0)
        alBufferData(buffer, sampler->getFormat(), bytes, read * sampler->getBytesPerSample(), sampler->getFrequency());
    delete bytes;
    return read > 0;
}

void Sound::unqueue() {
    ALint processed;
    alGetSourcei(source->binding->handle, AL_BUFFERS_PROCESSED, &processed);
    if (processed > 0) {
        ALuint handle[5];
        alSourceUnqueueBuffers(source->binding->handle, processed, handle);
        for (int i = 0; i < processed; ++i)
            available.push_back(handle[i]);
    }
}

void Sound::queue() {
    for (int i = 0; i < 5; ++i) {
        if (available.empty())
            return;
        ALuint next = available.back();
        if (!fill(next, 1 << 16, source->looping))
            return;
        available.pop_back();
        alSourceQueueBuffers(source->binding->handle, 1, &next);
    }
}

bool Sound::attach(Source * source) {
    
    // Buffered sound can always be attached
    if (!sampler) {
        alSourcei(source->binding->handle, AL_BUFFER, handle);
        return true;
    }
    
    // Streamed sound can only be attached to one source
    if (this->source)
        return false;
    
    // Attach source and queue buffers
    this->source = source;
    sampler->rewind();
    queue();
    return true;
}

void Sound::detach() {
    
    // Ignore if we are not attached to any source
    if (!source)
        return;
    
    // Unqueue all buffers and detach source
    unqueue();
    source = nullptr;
}

void Sound::update() {
    
    // Buffered sound does not need any update
    if (!sampler)
        return;
    
    // Unattached streamed sound, neither
    if (!source)
        return;
    
    // Update queue
    unqueue();
    queue();
}

#else

Sound::Sound(Sampler * sampler, bool stream) : sampler(stream ? sampler : nullptr) {}

Sound::~Sound() {
    delete sampler;
}

#endif
