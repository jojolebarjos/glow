
#ifndef GLOW_SOURCE_HPP
#define GLOW_SOURCE_HPP

#include "Common.hpp"
#include "Listener.hpp"
#include "Sound.hpp"
#include "Actor.hpp"

class Source : public AttachableActor {
    friend class Listener;
    friend class Sound;
public:

    Source(Source const &) = delete;
    Source & operator=(Source const &) = delete;

    void setLooping(bool looping);
    bool isLooping() const;
    
    // TODO direction, pitch, attenuation, gain

    void play();
    void pause();
    void resume();
    void stop();

    bool isPlaying() const;
    bool isPaused() const;

    // Note: the source is released as soon as the sound ends
    void release();

private:

    Source(Listener * listener);
    ~Source() = default;

    Listener * listener;
    std::list<Source *>::iterator iterator;
    bool released;

#ifndef GLOW_NO_OPENAL
    Listener::Binding * binding;
    bool paused;
#endif

    Sound * sound;
    bool looping;
    
};

#endif
