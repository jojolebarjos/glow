
#ifndef GLOW_WINDOW_HPP
#define GLOW_WINDOW_HPP

#include "Common.hpp"
#include "Texture.hpp"
#include "Framebuffer.hpp"

#ifdef GLOW_OPENVR
// See tunabrain's workaround for MinGW: https://github.com/ValveSoftware/openvr/issues/133
#include <openvr_mingw.hpp>
#endif

class Window {
public:
    
    Window();
    ~Window();
    
    Window(Window const &) = delete;
    Window & operator=(Window const &) = delete;
    
    bool initialize(uint32_t width, uint32_t height);
    
    GLFWwindow * getHandle() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    
    // TODO mouse
    // TODO keyboard
    // TODO gamepad
    
    bool hasStereoscopy() const;
    uint32_t getEyeWidth() const;
    uint32_t getEyeHeight() const;
    Framebuffer * getEyeFramebuffer(unsigned int index);
    glm::mat4 getEyeProjection(unsigned int index) const;
    glm::mat4 getEyeView(unsigned int index) const;
    // TODO more detailed getters for eyes?
    // TODO other tracking devices
    
    bool update();
    
private:

    GLFWwindow * window;
    uint32_t width;
    uint32_t height;
    
#ifdef GLOW_OPENVR
    
    vr::IVRSystem * hmd;
    
    uint32_t eye_width;
    uint32_t eye_height;
    Texture * eye_texture[2];
    Framebuffer * eye_framebuffer[2];
    glm::mat4 eye_projection[2];
    glm::mat4 eye_offset[2];
    glm::mat4 eye_view[2];
    
    vr::TrackedDevicePose_t device[vr::k_unMaxTrackedDeviceCount];
    glm::mat4 device_transform[vr::k_unMaxTrackedDeviceCount];
    glm::vec3 device_velocity[vr::k_unMaxTrackedDeviceCount];
    glm::vec3 device_angularVelocity[vr::k_unMaxTrackedDeviceCount];
    
    // TODO play area https://github.com/ValveSoftware/openvr/wiki/IVRChaperone_Overview
    
    glm::mat4 toGlm(vr::HmdMatrix34_t const & matrix) const;
    glm::mat4 toGlm(vr::HmdMatrix44_t const & matrix) const;
    glm::vec3 toGlm(vr::HmdVector3_t const & vector) const;
    
#endif
    
};

#endif
