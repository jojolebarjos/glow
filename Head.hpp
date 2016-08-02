
#ifndef GLOW_HEAD_HPP
#define GLOW_HEAD_HPP

#include "Actor.hpp"
#include "Camera.hpp"

class Head : public Actor {
    friend class Window;
public:
    
    Head(Head const &) = delete;
    Head & operator=(Head const &) = delete;
    
    glm::mat4 getTransform() const;
    glm::vec3 getVelocity() const;
    
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    
    Camera const * getEye(uint32_t index) const;
    Camera const * getEyeLeft() const;
    Camera const * getEyeRight() const;
    
    // TODO play area https://github.com/ValveSoftware/openvr/wiki/IVRChaperone_Overview
    
private:
    
    // Note: created and managed by Window
    Head() = default;
    ~Head() = default;
    
    glm::mat4 transform;
    glm::vec3 velocity;
    Camera left;
    Camera right;
    
    uint32_t width;
    uint32_t height;
    Texture texture[2];
    Framebuffer framebuffer[2];
    glm::mat4 offset[2];

};

#endif
