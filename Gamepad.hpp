
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
    
    boolx getAnyButton() const;
    virtual boolx getPrimaryButton() const;
    virtual boolx getSecondaryButton() const;
    // TODO up/down/right/left buttons and axis
    
protected:

    Gamepad() = default;
    virtual ~Gamepad() = default;
    
};

#endif
