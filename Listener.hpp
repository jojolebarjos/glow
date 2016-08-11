
#ifndef GLOW_LISTENER_HPP
#define GLOW_LISTENER_HPP

#include "Common.hpp"
#include "Actor.hpp"
#include "Sampler.hpp"

class Source;
class Sound;

class Listener : public AttachableActor {
    friend class Source;
    friend class Sound;
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
    
    //
    // http://kcat.strangesoft.net/openal-extensions/SOFT_HRTF.txt
    // https://github.com/kcat/openal-soft/blob/master/examples/alhrtf.c
    // http://www.bitoutsidethebox.com/shabda/hrtf-info/
    //
    
    enum struct ReverbPreset {
        NONE,
        // Based on AL/efx-presets.h
        GENERIC,
        CHAPEL,
        DUSTYROOM
    };
    
    Listener();
    ~Listener();
    
    Listener(Listener const &) = delete;
    Listener & operator=(Listener const &) = delete;
    
    bool initialize();
    void update();
    
    // TODO physical properties: doppler alDopplerFactor, alDopplerVelocity, alSpeedOfSound, alDistanceModel
   
    // Note: a stream can only used by one source at a time
    Sound * addSoundBuffer(Sampler const & sampler);
    Sound * addSoundStream(Sampler const & sampler);
    
    Source * addSource(Sound * sound);
    // TODO add source priority system to avoid hard limitation and reduce CPU usage?
    
    // TODO add sound group
    // - can play/pause/stop whole group
    // - source can be relative to player, relative to group, or absolute
    // - group can have parent group
    // - releasing a group release everything
    
    // TODO improve effects quality and control
    void setReverb(ReverbPreset preset);
    
private:

#ifndef GLOW_NO_OPENAL
    
    ALCdevice * device;
    ALCcontext * context;
    bool efx;
    bool hrtf;
    
    struct Binding {
        ALuint handle;
        Source * source;
    };
    std::vector<Binding *> bindings;
    
    ReverbPreset reverb_preset;
    ALuint reverb_effect;
    ALuint reverb_slot;
    
#endif

    std::list<Sound *> sounds;
    std::list<Source *> sources;
    
};

#endif
