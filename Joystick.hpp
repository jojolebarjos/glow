
#ifndef GLOW_JOYSTICK_HPP
#define GLOW_JOYSTICK_HPP

#include "Gamepad.hpp"

class Joystick : public Gamepad {
public:
    
    Joystick(int index);
    
    Joystick(Joystick const &) = delete;
    Joystick & operator=(Joystick const &) = delete;
    
    bool isConnected() const;
    
    uint32_t getAxisCount() const;
    float getAxis(uint32_t id) const;
    
    uint32_t getButtonCount() const;
    boolx getButton(uint32_t id) const;
    
    boolx getPrimaryButton() const;
    boolx getSecondaryButton() const;
    
    void update();
    
private:
    
    int index;
    bool connected;
    uint32_t axis_count;
    float axis[16];
    uint32_t button_count;
    int current;
    bool button[2][16];
    
};

#endif
