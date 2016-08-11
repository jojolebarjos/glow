
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
    
    bool isStream() const;
    // TODO keep infos about sound (format, duration...)?
    
    // TODO release?
    
private:
    
    Sound(Sampler const & sampler, bool stream);
    ~Sound();
    
    Sampler * sampler;
    
#ifndef GLOW_NO_OPENAL
    
    uint32_t fill(ALuint buffer, uint32_t samples, bool loop);
    
    void unqueue();
    void queue();
    
    bool attach(Source * source);
    void detach();
    
    void update();
    
    ALuint handle;
    
    Source * source;
    ALuint cycle[5];
    std::vector<ALuint> available;
    
#endif

};

#endif
