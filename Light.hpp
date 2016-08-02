
#ifndef GLOW_LIGHT_HPP
#define GLOW_LIGHT_HPP

#include "Actor.hpp"

class Light : public AttachableActor {
    friend class Renderer;
public:
    
    // TODO other types of lights (directional, spot...)
    
    float getRadius() const;
    void setRadius(float radius);
    
    glm::vec3 getColor() const;
    void setColor(glm::vec3 const & color);
    
    // TODO visibility tests
    
private:

    float radius;
    glm::vec3 color;
    
};

#endif
