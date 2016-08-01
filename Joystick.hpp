
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
    float getAxisValue(uint32_t id) const;
    
    uint32_t getButtonCount() const;
    bool isButtonDown(uint32_t id) const;
    bool isButtonPressed(uint32_t id) const;
    bool isButtonReleased(uint32_t id) const;
    
    void update();
    
private:
    
    int index;
    bool connected;
    uint32_t axis_count;
    float axis[16];
    uint32_t button_count;
    bool button[16];
    
};

#endif
