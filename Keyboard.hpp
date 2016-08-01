
#ifndef GLOW_KEYBOARD_HPP
#define GLOW_KEYBOARD_HPP

#include "Gamepad.hpp"

class Window;

class Keyboard : public Gamepad {
public:
    
    Keyboard(Window * window);
    
    Keyboard(Keyboard const &) = delete;
    Keyboard & operator=(Keyboard const &) = delete;
    
    bool isConnected() const;
    
    uint32_t getButtonCount() const;
    bool isButtonDown(uint32_t id) const;
    bool isButtonPressed(uint32_t id) const;
    bool isButtonReleased(uint32_t id) const;
    
    // TODO text to key id conversion?
    // TODO text input? clipboard?
    
    void update();
    
private:

    Window * window;
    int current;
    char button[2][GLFW_KEY_LAST + 1];
    
};

#endif
