
#ifndef GLOW_SOURCE_HPP
#define GLOW_SOURCE_HPP

#include "Common.hpp"
#include "Listener.hpp"
#include "Sound.hpp"

class Source {
    friend class Listener;
    friend class Sound;
public:

    Source(Source const &) = delete;
    Source & operator=(Source const &) = delete;

    void setLooping(bool looping);
    bool isLooping() const;
    
    void setPosition(glm::vec3 const & position);
    glm::vec3 getPosition() const;
    
    void setVelocity(glm::vec3 const & velocity);
    glm::vec3 getVelocity() const;

    // TODO relative, direction, pitch, attenuation, gain

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

#ifndef GLOW_NO_OPENAL
    Listener::Binding * binding;
#endif

    Sound * sound;
    bool looping;
    glm::vec3 position;
    glm::vec3 velocity;
    
};

#endif
