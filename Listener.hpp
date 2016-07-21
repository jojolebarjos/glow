
#ifndef GLOW_LISTENER_HPP
#define GLOW_LISTENER_HPP

#include "Common.hpp"
#include "Sampler.hpp"

class Listener {
    struct Source;
public:
    
    // See https://www.openal.org/documentation/OpenAL_Programmers_Guide.pdf
    
    class Sound {
        friend class Listener;
    public:
       
        Sound(Sound const &) = delete;
        Sound & operator=(Sound const &) = delete;
        
        void setPosition(glm::vec3 const & position);
        glm::vec3 getPosition() const;
        
        // TODO relative, velocity, direction, pitch, attenuation, gain, looping
        
        void play();
        // TODO allow pause/resume?
        void stop();
        
        bool isPlaying() const;
        
        // Note: the source is released as soon as the sound ends
        void release();
        
        // TODO effects using EFX
        // http://kcat.strangesoft.net/misc-downloads/Effects%20Extension%20Guide.pdf
        // https://github.com/kcat/openal-soft/blob/master/include/AL/efx.h
        // ideas:
        //  - distant sound http://filmsound.org/QA/distantsounds.htm
        
    private:
        
        Sound(Listener * listener);
        ~Sound() = default;
        
        Listener * listener;
        std::list<Sound *>::iterator iterator;
        bool released;
        
        Source * source;
        
        uint32_t buffer;
        glm::vec3 position;
        
    };
    
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
    
    uint32_t addSoundBuffer(Sampler & sampler);
    Sound * addSound(uint32_t buffer);
    
    // TODO play streamed sound (a.k.a. music) using circular buffer
    
private:

    ALCdevice * device;
    ALCcontext * context;

    struct Buffer {
        ALuint handle;
        // TODO store infos?
    };
    std::vector<Buffer> buffers;
    
    struct Source {
        ALuint handle;
        Sound * sound;
    };
    std::vector<Source *> sources;
    
    std::list<Sound *> sounds;
    
};

#endif
