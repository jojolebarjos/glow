
#include "Listener.hpp"
#include "Source.hpp"
#include "Sound.hpp"

Listener::Listener()
#ifndef GLOW_NO_OPENAL
: device(nullptr), context(nullptr), efx(false), hrtf(false)
#endif
{}

Listener::~Listener() {
#ifndef GLOW_NO_OPENAL
    if (device) {
        if (context) {
            
            // Release resources
            for (Binding * binding : bindings) {
                alSourceStop(binding->handle);
                alDeleteSources(1, &binding->handle);
                delete binding;
            }
            for (Sound * sound : sounds)
                delete sound;
            for (Source * source : sources)
                delete source;
            if (efx) {
                alDeleteAuxiliaryEffectSlots(1, &reverb_slot);
                alDeleteEffects(1, &reverb_effect);
            }
            
            // Delete context
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(context);
        }
        alcCloseDevice(device);
    }
#endif
}

bool Listener::initialize() {
#ifndef GLOW_NO_OPENAL
    
    // Only one context is allowed
    if (alcGetCurrentContext()) {
        std::cout << "Only one OpenAL context can be active at a time!" << std::endl;
        return false;
    }
    
    // Open device
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cout << "Failed to create OpenAL device" << std::endl;
        return false;
    }
    
    // Check EFX
    efx = alcIsExtensionPresent(device, ALC_EXT_EFX_NAME) == AL_TRUE;
    if (efx) {
        ALCint major, minor;
        alcGetIntegerv(device, ALC_EFX_MAJOR_VERSION, 1, &major);
        alcGetIntegerv(device, ALC_EFX_MINOR_VERSION, 1, &minor);
        std::cout << "OpenAL EFX: " << major << '.' << minor << std::endl;
    } else
        std::cout << "OpenAL EFX: <none>" << std::endl;
    
    // Check HRTF
    hrtf = alcIsExtensionPresent(device, "ALC_SOFT_HRTF");
    if (hrtf) {
        ALCint hrtf_count;
        alcGetIntegerv(device, ALC_NUM_HRTF_SPECIFIERS_SOFT, 1, &hrtf_count);
        std::cout << "OpenAL Soft HRTF:";
        for (int i = 0; i < hrtf_count; ++i) {
            const ALCchar * hrtf_name = alcGetStringiSOFT(device, ALC_HRTF_SPECIFIER_SOFT, i);
            std::cout << ' ' << hrtf_name;
        }
        std::cout << std::endl;
        // TODO select an HRTF
    } else
        std::cout << "OpenAL Soft HRTF: <none>" << std::endl;
    
    // Create context
    ALint attribs[6] = {0};
    int attribs_offset = 0;
    if (efx) {
        attribs[attribs_offset++] = ALC_MAX_AUXILIARY_SENDS;
        attribs[attribs_offset++] = 4;
    }
    if (hrtf) {
        attribs[attribs_offset++] = ALC_HRTF_SOFT;
        attribs[attribs_offset++] = ALC_TRUE;
        // TODO select desired HRTF (ALC_HRTF_ID_SOFT)
        alcResetDeviceSOFT(device, attribs);
        ALCint hrtf_state;
        alcGetIntegerv(device, ALC_HRTF_SOFT, 1, &hrtf_state);
        if (hrtf_state) {
            std::cout << "OpenAL Soft HRTF: " << alcGetString(device, ALC_HRTF_SPECIFIER_SOFT) << std::endl;
        } else
            std::cout << "OpenAL Soft HRTF: failed to enable" << std::endl;
    }
    context = alcCreateContext(device, attribs);
    if (!context || !alcMakeContextCurrent(context)) {
        std::cout << "Failed to create OpenAL context" << std::endl;
        if (context) {
            alcDestroyContext(context);
            context = nullptr;
        }
        alcCloseDevice(device);
        device = nullptr;
        return false;
    }
    
    // Print version
    std::cout << "OpenAL version: " << alGetString(AL_VERSION) << std::endl;
    std::cout << "OpenAL renderer: " << alGetString(AL_RENDERER) << std::endl;
    
    // Allocate sources
    uint32_t max_bindings = 256; // TODO define from arguments
    for (uint32_t i = 0; i < max_bindings; ++i) {
        ALuint handle;
        alGenSources(1, &handle);
        assert(handle);
        bindings.push_back(new Binding({handle, nullptr}));
    }
    
    // Prepare reverb effect
    if (efx) {
        
        // Create slot
        alGenAuxiliaryEffectSlots(1, &reverb_slot);
        assert(reverb_slot);
        alAuxiliaryEffectSloti(reverb_slot, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
        reverb_preset = ReverbPreset::NONE;
        
        // Create effect
        alGenEffects(1, &reverb_effect);
        assert(reverb_effect);
        alEffecti(reverb_effect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
        if (alGetError() != AL_NO_ERROR)
            std::cout << "EAX REVERB NOT SUPPORTED" << std::endl;
        else
            std::cout << "reverb none" << std::endl;
        
        // Connect all sources to effect slot
        for (uint32_t i = 0; i < 1; ++i)
            alSource3i(bindings[i]->handle, AL_AUXILIARY_SEND_FILTER, reverb_slot, 0, AL_FILTER_NULL);
    }
    
    assert(alGetError() == AL_NO_ERROR);
    
    return true;
    
#else
    std::cout << "OpenAL is not available" << std::endl;
    return false;
#endif
}

void Listener::update() {
#ifndef GLOW_NO_OPENAL
    
    // Update listener location
    glm::mat4 transform = getTransform();
    glm::vec3 position(transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    glm::vec3 forward(transform * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    glm::vec3 up(transform * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    alListener3f(AL_POSITION, position.x, position.y, position.z);
    ALfloat orientation[] = {forward.x, forward.y, forward.z, up.x, up.y, up.z};
    alListenerfv(AL_ORIENTATION, orientation);
    glm::vec3 velocity = getVelocity();
    alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    
    // Update sources location
    for (Source * source : sources)
        if (source->binding) {
            // TODO do not call OpenAL if nothing changed
            glm::vec3 position = source->getPosition();
            alSource3f(source->binding->handle, AL_POSITION, position.x, position.y, position.z);
            glm::vec3 velocity = source->getVelocity();
            alSource3f(source->binding->handle, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
        }
    
    // TODO reduce CPU overload by doing these checks at a lower frequency
    
    // Check if sources have ended
    ALint state;
    for (Binding * binding : bindings)
        if (binding->source) {
            alGetSourcei(binding->handle, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING)
                binding->source->stop();
        }
    
    // Update streamed sounds
    for (Sound * sound : sounds)
        sound->update();
    
#endif
}

Sound * Listener::addSoundBuffer(Sampler const & sampler) {
#ifndef GLOW_NO_OPENAL
    if (device) {
        Sound * sound = new Sound(sampler, false);
        sounds.push_front(sound);
        return sound;
    }
#endif
    return nullptr;
}

Sound * Listener::addSoundStream(Sampler const & sampler) {
#ifndef GLOW_NO_OPENAL
    if (device) {
        Sound * sound = new Sound(sampler, true);
        sounds.push_front(sound);
        return sound;
    }
#endif
    return nullptr;
}

Source * Listener::addSource(Sound * sound) {
#ifndef GLOW_NO_OPENAL
    if (device && sound) {
        Source * source = new Source(this);
        sources.push_front(source);
        source->iterator = sources.begin();
        source->sound = sound;
        return source;
    }
#endif
    return nullptr;
}

void Listener::setReverb(ReverbPreset preset) {
#ifndef GLOW_NO_OPENAL
    if (efx && preset != reverb_preset) {
        reverb_preset = preset;
        
        // Disable reverb if requested
        if (preset == ReverbPreset::NONE) {
            alAuxiliaryEffectSloti(reverb_slot, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
            std::cout << "reverb none" << std::endl;
        }
        
        // Select preset and upload parameters
        else {
            EFXEAXREVERBPROPERTIES properties;
            switch (preset) {
                case ReverbPreset::CHAPEL:
                    properties = EFX_REVERB_PRESET_CHAPEL;
                    std::cout << "reverb chapel" << std::endl;
                    break;
                case ReverbPreset::DUSTYROOM:
                    properties = EFX_REVERB_PRESET_DUSTYROOM;
                    std::cout << "reverb dusty room" << std::endl;
                    break;
                default:
                    properties = EFX_REVERB_PRESET_GENERIC;
                    std::cout << "reverb generic" << std::endl;
            }
            alEffectf(reverb_effect, AL_EAXREVERB_DENSITY, properties.flDensity);
            alEffectf(reverb_effect, AL_EAXREVERB_DIFFUSION, properties.flDiffusion);
            alEffectf(reverb_effect, AL_EAXREVERB_GAIN, properties.flGain);
            alEffectf(reverb_effect, AL_EAXREVERB_GAINHF, properties.flGainHF);
            alEffectf(reverb_effect, AL_EAXREVERB_GAINLF, properties.flGainLF);
            alEffectf(reverb_effect, AL_EAXREVERB_DECAY_TIME, properties.flDecayTime);
            alEffectf(reverb_effect, AL_EAXREVERB_DECAY_HFRATIO, properties.flDecayHFRatio);
            alEffectf(reverb_effect, AL_EAXREVERB_DECAY_LFRATIO, properties.flDecayLFRatio);
            alEffectf(reverb_effect, AL_EAXREVERB_REFLECTIONS_GAIN, properties.flReflectionsGain);
            alEffectf(reverb_effect, AL_EAXREVERB_REFLECTIONS_DELAY, properties.flReflectionsDelay);
            alEffectfv(reverb_effect, AL_EAXREVERB_REFLECTIONS_PAN, properties.flReflectionsPan);
            alEffectf(reverb_effect, AL_EAXREVERB_LATE_REVERB_GAIN, properties.flLateReverbGain);
            alEffectf(reverb_effect, AL_EAXREVERB_LATE_REVERB_DELAY, properties.flLateReverbDelay);
            alEffectfv(reverb_effect, AL_EAXREVERB_LATE_REVERB_PAN, properties.flLateReverbPan);
            alEffectf(reverb_effect, AL_EAXREVERB_ECHO_TIME, properties.flEchoTime);
            alEffectf(reverb_effect, AL_EAXREVERB_ECHO_DEPTH, properties.flEchoDepth);
            alEffectf(reverb_effect, AL_EAXREVERB_MODULATION_TIME, properties.flModulationTime);
            alEffectf(reverb_effect, AL_EAXREVERB_MODULATION_DEPTH, properties.flModulationDepth);
            alEffectf(reverb_effect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, properties.flAirAbsorptionGainHF);
            alEffectf(reverb_effect, AL_EAXREVERB_HFREFERENCE, properties.flHFReference);
            alEffectf(reverb_effect, AL_EAXREVERB_LFREFERENCE, properties.flLFReference);
            alEffectf(reverb_effect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, properties.flRoomRolloffFactor);
            alEffecti(reverb_effect, AL_EAXREVERB_DECAY_HFLIMIT, properties.iDecayHFLimit);
            alAuxiliaryEffectSloti(reverb_slot, AL_EFFECTSLOT_EFFECT, reverb_effect);
        }
    }
#endif    
}
