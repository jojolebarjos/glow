
#ifndef GLOW_WINDOW_HPP
#define GLOW_WINDOW_HPP

#include "Common.hpp"
#include "Texture.hpp"
#include "Framebuffer.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"
#include "Mesh.hpp"

#include "Gamepad.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "Joystick.hpp"
#include "Camera.hpp"
#include "Head.hpp"
#include "Controller.hpp"

class Window {
public:
    
    Window();
    ~Window();
    
    Window(Window const &) = delete;
    Window & operator=(Window const &) = delete;
    
    bool initialize(uint32_t width, uint32_t height, bool stereoscopy = false, bool debug = false);
    
    GLFWwindow * getHandle() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    // TODO get framebuffer
    
    float getTime() const;
    float getDeltaTime() const;
    
    bool hasFocus() const;
    
    Mouse const * getMouse() const;
    Keyboard const * getKeyboard() const;
    Joystick const * getJoystick(uint32_t index) const;
    
    Head const * getHead() const;
    Controller const * getController(uint32_t index) const;
    // TODO get reference
    
    bool update();
    
private:

    GLFWwindow * window;
    uint32_t width;
    uint32_t height;
    
    double time;
    double dt;
    
    Mouse * mouse;
    Keyboard * keyboard;
    Joystick * joystick[4];
    Head * head;
    Controller * controller[2];
    
#ifndef GLOW_NO_OPENVR
    
    vr::IVRSystem * hmd;
    vr::TrackedDevicePose_t device[vr::k_unMaxTrackedDeviceCount];
    vr::ETrackedDeviceClass device_type[vr::k_unMaxTrackedDeviceCount];
    
    glm::mat4 toGlm(vr::HmdMatrix34_t const & matrix) const;
    glm::mat4 toGlm(vr::HmdMatrix44_t const & matrix) const;
    glm::vec3 toGlm(vr::HmdVector3_t const & vector) const;
    
#endif
    
};

#endif
