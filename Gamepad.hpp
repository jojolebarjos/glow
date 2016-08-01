
#ifndef GLOW_GAMEPAD_HPP
#define GLOW_GAMEPAD_HPP

#include "Common.hpp"

class Gamepad {
public:
    
    static Gamepad const * getNull();
    
    Gamepad(Gamepad const &) = delete;
    Gamepad & operator=(Gamepad const &) = delete;
    
    virtual bool isConnected() const;
    // TODO get name?
    
    virtual uint32_t getAxisCount() const;
    virtual float getAxisValue(uint32_t id) const;
    // TODO axis name?
    // TODO axis value min/max?
    
    virtual uint32_t getButtonCount() const;
    virtual bool isButtonDown(uint32_t id) const;
    virtual bool isButtonPressed(uint32_t id) const;
    virtual bool isButtonReleased(uint32_t id) const;
    // TODO button name? enumerate buttons?
    
    // TODO get if any/primary/secondary/up/down... button is pressed
    
protected:

    Gamepad() = default;
    virtual ~Gamepad() = default;
    
};

#endif
