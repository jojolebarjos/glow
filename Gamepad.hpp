
#ifndef GLOW_GAMEPAD_HPP
#define GLOW_GAMEPAD_HPP

#include "Common.hpp"

class Gamepad {
public:
    
    static Gamepad const * getNull();
    
    virtual bool isConnected() const;
    // TODO get name?
    
    virtual uint32_t getAxisCount() const;
    virtual float getAxis(uint32_t id) const;
    // TODO axis name?
    // TODO axis value min/max?
    
    virtual uint32_t getButtonCount() const;
    virtual boolx getButton(uint32_t id) const;
    // TODO button name? enumerate buttons?
    // TODO button touched (for controllers)?
    
    boolx getAnyButton() const;
    virtual boolx getPrimaryButton() const;
    virtual boolx getSecondaryButton() const;
    // TODO tertiary? up/down/right/left buttons and axis
    
    // TODO haptic feedback
    
protected:

    Gamepad() = default;
    virtual ~Gamepad() = default;
    
};

#endif
