
#ifndef GLOW_CAMERA_HPP
#define GLOW_CAMERA_HPP

#include "Actor.hpp"
#include "Framebuffer.hpp"

class Camera : public AttachableActor {
public:
    
    Camera();
    
    glm::mat4 getProjection() const;
    void setProjection(glm::mat4 const & projection);
    
    glm::mat4 getView() const;
    
    // TODO viewport (x, y, width, height)
    // TODO fov, clipping planes...?
    
    Framebuffer * getFramebuffer() const;
    void setFramebuffer(Framebuffer * framebuffer);
    
private:
    
    glm::mat4 projection;
    Framebuffer * framebuffer;
    
};

#endif
