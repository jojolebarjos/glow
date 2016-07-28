
#ifndef GLOW_SOUND_HPP
#define GLOW_SOUND_HPP

#include "Listener.hpp"
#include "Sampler.hpp"

class Source;

class Sound {
    friend class Listener;
    friend class Source;
public:
    
    Sound(Sound const &) = delete;
    Sound & operator=(Sound const &) = delete;
    
    // TODO keep infos about sound (format, duration...)?
    
    // TODO release?
    
private:
    
    Sound(Sampler * sampler);
    // TODO streaming sampler constructor
    ~Sound();
    
#ifdef GLOW_OPENAL
    
    ALuint handle;
    
    // TODO streaming buffers
    // TODO flag to know whether the stream is playing, to forbid multiple sources
    
#endif

};

#endif
