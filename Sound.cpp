
#include "Sound.hpp"
#include "Source.hpp"

#ifndef GLOW_NO_OPENAL

Sound::Sound(Sampler * sampler) {
    
    // Create buffer
    alGenBuffers(1, &handle);
    assert(handle);
    
    // Read bytes
    uint32_t length = sampler->getBytes();
    char * bytes = new char[length];
    sampler->rewind();
    sampler->read(bytes, sampler->getSize());
    
    // Store data
    alBufferData(handle, sampler->getFormat(), bytes, length, sampler->getFrequency());
}

Sound::~Sound() {
    alDeleteBuffers(1, &handle);
}

#else

Sound::Sound(Sampler * sampler) {}

Sound::~Sound() {}

#endif
