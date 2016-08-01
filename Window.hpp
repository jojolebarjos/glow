
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

#ifndef GLOW_NO_OPENVR
// See tunabrain's workaround for MinGW: https://github.com/ValveSoftware/openvr/issues/133
#include <openvr_mingw.hpp>
#endif

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
    
    float getTime() const;
    float getDeltaTime() const;
    
    bool hasFocus() const;
    
    Mouse const * getMouse() const;
    Keyboard const * getKeyboard() const;
    Joystick const * getJoystick(uint32_t index) const;
    
    bool hasStereoscopy() const;
    uint32_t getEyeWidth() const;
    uint32_t getEyeHeight() const;
    Framebuffer * getEyeFramebuffer(unsigned int index);
    glm::mat4 getEyeProjection(unsigned int index) const;
    glm::mat4 getEyeView(unsigned int index) const;
    
    bool isDeviceConnected(uint32_t index) const;
    bool isDeviceHead(uint32_t index) const;
    bool isDeviceController(uint32_t index) const;
    bool isDeviceReference(uint32_t index) const;
    uint32_t getDeviceHead() const;
    uint32_t getDeviceController(uint32_t id) const;
    uint32_t getDeviceReference(uint32_t id) const;
    glm::vec3 getDevicePosition(uint32_t index) const;
    glm::mat4 getDeviceTransform(uint32_t index) const;
    glm::vec3 getDeviceVelocity(uint32_t index) const;
    glm::vec3 getDeviceAngularVelocity(uint32_t index) const;
    
    // TODO get buttons/axis (i.e. should we merge that with gamepads?)
    bool isDeviceButtonDown(uint32_t index, uint32_t button_index) const;
    
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
    
#ifndef GLOW_NO_OPENVR
    
    vr::IVRSystem * hmd;
    
    uint32_t eye_width;
    uint32_t eye_height;
    Texture * eye_texture[2];
    Framebuffer * eye_framebuffer[2];
    glm::mat4 eye_projection[2];
    glm::mat4 eye_offset[2];
    glm::mat4 eye_view[2];
    
    vr::TrackedDevicePose_t device[vr::k_unMaxTrackedDeviceCount];
    vr::ETrackedDeviceClass device_type[vr::k_unMaxTrackedDeviceCount];
    glm::mat4 device_transform[vr::k_unMaxTrackedDeviceCount];
    glm::vec3 device_velocity[vr::k_unMaxTrackedDeviceCount];
    glm::vec3 device_angularVelocity[vr::k_unMaxTrackedDeviceCount];
    vr::VRControllerState_t state[vr::k_unMaxTrackedDeviceCount];
    
    // TODO play area https://github.com/ValveSoftware/openvr/wiki/IVRChaperone_Overview
    
    glm::mat4 toGlm(vr::HmdMatrix34_t const & matrix) const;
    glm::mat4 toGlm(vr::HmdMatrix44_t const & matrix) const;
    glm::vec3 toGlm(vr::HmdVector3_t const & vector) const;
    
#endif
    
};

#endif
