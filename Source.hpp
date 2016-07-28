
#ifndef GLOW_SOURCE_HPP
#define GLOW_SOURCE_HPP

#include "Common.hpp"
#include "Listener.hpp"
#include "Sound.hpp"

class Source {
    friend class Listener;
public:

    Source(Source const &) = delete;
    Source & operator=(Source const &) = delete;

    // TODO add setSound here instead of in Listener addSource
    
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

#ifdef GLOW_OPENAL
    
    Listener * listener;
    std::list<Source *>::iterator iterator;
    bool released;

    Listener::Binding * binding;

    Sound * sound;
    glm::vec3 position;

#endif
    
};

#endif
