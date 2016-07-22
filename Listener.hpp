
#ifndef GLOW_LISTENER_HPP
#define GLOW_LISTENER_HPP

#include "Common.hpp"
#include "Sampler.hpp"

class Listener {
    struct Binding;
public:
    
    //
    // https://www.openal.org/documentation/OpenAL_Programmers_Guide.pdf
    // http://kcat.strangesoft.net/misc-downloads/Effects%20Extension%20Guide.pdf
    //
    // Effect Extension:
    //  - EFX extension must be checked (ALC_EXT_EFX_NAME)
    //  - Context should be requested with some hints (e.g. ALC_MAX_AUXILIARY_SENDS)
    //  - Auxiliary effect slot can be attached to multiple sources, but can only have one
    //    effect. To apply multiple effects on a source (or group of sources), multiple
    //    slots must be used (but sources may have an limit on how many slot they can send to)
    //  - Auxiliary effect slot can have a specific gain
    //  - Effects (null, reverb, chorus, distortion, echo, flanger...) acts as a preset object.
    //    They can be configured offline, and then attached to a slot. To modify an effect
    //    after being attached to a source has no effect on the source (i.e. it must be reattached
    //    to apply changes)
    //  - Filters (lowpass, bandpass, highpass) can be attached to a source (direct filter)
    //    or to an auxiliary slot. Like effects, they are presets and need to be attached again
    //    to apply changes.
    //  - Additional source/listener properties are added by EFX (air absorbtion...)
    //
    // Concepts and hints:
    //  - Reverb and air absorbtion can give audio cues and are importants
    //  - Sound obstruction occurs when a sound in the same room is partially or
    //    completely muffled. This causes sound diffraction, i.e. high frequencies
    //    are filtered out. Reverberation are mostly unaffected.
    //  - Sound occlusion is when there is no air path between the sound and the
    //    listener. Lowpass and attenuation are used.
    //  - Sound exclusion, same scenario as sound occlusion, except that there is
    //    a small opening. Hence, no lowpass, a bit of attenuation and the sound
    //    get the reverb of the listener's room.
    //  - Occlusion and environment zones are likely to be related to some high
    //    level design/logic, and associated to physics engine.
    //  - Smooth transition between zones are required to avoid audio artifacts
    //  - Audio framerate, due to the "low-resolution" of the human ear, can be
    //    lower than graphical framerate (30FPS is enough). Hence, CPU time can
    //    be saved by reducing the frequency of audio computations :
    //     * Sound playback and pitch adjustment: 60Hz
    //     * Listener/source position/direction: 30Hz
    //     * Reverb/reflection panning vectors: 30Hz
    //     * Reverberation parameters adjustment: 15Hz
    //
    
    class Source {
        friend class Listener;
    public:
       
        Source(Source const &) = delete;
        Source & operator=(Source const &) = delete;
        
        void setPosition(glm::vec3 const & position);
        glm::vec3 getPosition() const;
        
        // TODO relative, velocity, direction, pitch, attenuation, gain, looping
        
        void play();
        // TODO allow pause/resume?
        void stop();
        
        bool isPlaying() const;
        
        // Note: the source is released as soon as the sound ends
        void release();
        
    private:
        
        Source(Listener * listener);
        ~Source() = default;
        
        Listener * listener;
        std::list<Source *>::iterator iterator;
        bool released;
        
        Binding * binding;
        
        uint32_t sound;
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
    
    // TODO make a public class for this Buffer
    uint32_t addSoundBuffer(Sampler & sampler);
    // TODO add sound stream (i.e. can be used transparently by sounds, no special Music class)
    
    Source * addSource(uint32_t sound);
    
private:

    ALCdevice * device;
    ALCcontext * context;
    bool efx;

    struct Sound {
        ALuint handle;
        // TODO store infos?
    };
    std::vector<Sound> sounds;
    
    struct Binding {
        ALuint handle;
        Source * source;
    };
    std::vector<Binding *> bindings;
    
    std::list<Source *> sources;
    
};

#endif