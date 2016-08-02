
#ifndef GLOW_CONTROLLER_HPP
#define GLOW_CONTROLLER_HPP

#include "Actor.hpp"
#include "Gamepad.hpp"

class Controller : public Actor, public Gamepad {
    friend class Window;
public:
    
    // TODO provide button IDs
    
    Controller(Controller const &) = delete;
    Controller & operator=(Controller const &) = delete;
    
    bool isConnected() const;
    
    glm::mat4 getTransform() const;
    glm::vec3 getVelocity() const;
    
    uint32_t getAxisCount() const;
    float getAxis(uint32_t id) const;
    
    uint32_t getButtonCount() const;
    boolx getButton(uint32_t id) const;
    
    boolx getPrimaryButton() const;
    boolx getSecondaryButton() const;
    
private:
    
    // Note: created and managed by Window
    Controller();
    ~Controller() = default;

    int index;
    glm::mat4 transform;
    glm::vec3 velocity;
#ifndef GLOW_NO_OPENVR
    int current;
    vr::VRControllerState_t state[2];
#endif
    
};

#endif
