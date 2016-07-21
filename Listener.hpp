
#ifndef GLOW_LISTENER_HPP
#define GLOW_LISTENER_HPP

#include "Common.hpp"
#include "Sampler.hpp"

class Listener {
public:
    
    // See https://www.openal.org/documentation/OpenAL_Programmers_Guide.pdf
    
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
    
    uint32_t addSound(Sampler & sampler);
    
    int32_t playSound(uint32_t sound, glm::vec3 const & position);
    
    
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

    struct Sound {
        ALuint handle;
        // TODO what is needed?
    };
    std::vector<Sound> sounds;
    
    struct Source {
        ALuint handle;
        // TODO what is needed?
    };
    std::vector<Source> sources;
    
    // TODO keep a list of virtual sources
    // TODO can be either managed (i.e. they disappear when they finish) or unmanaged (i.e. the user need to do something) sources
    
};

#endif
