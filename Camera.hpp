
#ifndef GLOW_CAMERA_HPP
#define GLOW_CAMERA_HPP

#include "Actor.hpp"
#include "Framebuffer.hpp"

class Camera : public Actor {
public:
    
    Camera();
    
    Camera(Camera const &) = delete;
    Camera & operator=(Camera const &) = delete;
    
    glm::mat4 getTransform() const;
    void setTransform(glm::mat4 const & transform);
    
    glm::vec3 getVelocity() const;
    void setVelocity(glm::vec3 const & velocity);
    
    glm::mat4 getProjection() const;
    void setProjection(glm::mat4 const & projection);
    
    glm::mat4 getView() const;
    void setView(glm::mat4 const & view);
    
    Framebuffer * getFramebuffer() const;
    void setFramebuffer(Framebuffer * framebuffer);
    
private:
    
    glm::mat4 transform;
    glm::vec3 velocity;
    glm::mat4 projection;
    glm::mat4 view;
    Framebuffer * framebuffer;
    
};

#endif
