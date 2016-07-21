
#ifndef GLOW_LISTENER_HPP
#define GLOW_LISTENER_HPP

#include "Common.hpp"

class Listener {
public:
    
    Listener();
    ~Listener();
    
    Listener(Listener const &) = delete;
    Listener & operator=(Listener const &) = delete;
    
    bool initialize();
    void update();
    
    void setPosition(glm::vec3 const & position);
    glm::vec3 getPosition();
    
    void setOrientation(glm::vec3 const & forward, glm::vec3 const & up);
    // TODO get orientation
    
    // TODO AL_VELOCITY
    
    // TODO physical properties: doppler alDopplerFactor, alDopplerVelocity, alSpeedOfSound, alDistanceModel
    
    // TODO play sound
    // TODO hide playback behind virtual sources to avoid limitations
    
    // TODO play streamed sound (a.k.a. music) using circular buffer
    
    // TODO effects using EFX?
    // http://kcat.strangesoft.net/misc-downloads/Effects%20Extension%20Guide.pdf
    // https://github.com/kcat/openal-soft/blob/master/include/AL/efx.h
    // ideas:
    //  - distant sound http://filmsound.org/QA/distantsounds.htm
    
private:

    ALCdevice * device;
    ALCcontext * context;
    
    // TODO cache data (e.g. listener infos)?)

};

#endif
