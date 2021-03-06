
#ifndef GLOW_MOUSE_HPP
#define GLOW_MOUSE_HPP

#include "Gamepad.hpp"

class Window;

class Mouse : public Gamepad {
public:
    
    Mouse(Window * window);
    
    Mouse(Mouse const &) = delete;
    Mouse & operator=(Mouse const &) = delete;
    
    bool isConnected() const;
    
    uint32_t getAxisCount() const;
    float getAxis(uint32_t id) const;
    
    uint32_t getButtonCount() const;
    boolx getButton(uint32_t id) const;
    
    boolx getPrimaryButton() const;
    boolx getSecondaryButton() const;
    
    glm::vec2 getPosition() const;
    
    // TODO mouse capture?
    // TODO scroll?
    
    void update();
    
private:

    Window * window;
    glm::vec2 position;
    int current;
    char button[2][GLFW_MOUSE_BUTTON_LAST + 1];
    
};

#endif
