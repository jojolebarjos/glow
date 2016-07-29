
#ifndef GLOW_WINDOW_HPP
#define GLOW_WINDOW_HPP

#include "Common.hpp"
#include "Texture.hpp"
#include "Framebuffer.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"
#include "Mesh.hpp"

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
    
    // TODO distinguish whether a button was just pressed/released (i.e. changed from last frame)
    
    float getTime() const;
    float getDeltaTime() const;
    
    bool hasFocus() const;
    
    bool isMouseButtonDown(uint32_t index) const;
    // TODO mouse scroll
    glm::vec2 getMouseLocation() const;
    // TODO mouse capture?
    
    bool isKeyboardButtonDown(uint32_t id) const;
    // TODO text input? clipboard?
    
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
    
    double time;
    double dt;
    
#ifdef GLOW_OPENVR
    
    vr::IVRSystem * hmd;
    
    uint32_t eye_width;
    uint32_t eye_height;
    Texture * eye_texture[2];
    Framebuffer * eye_framebuffer[2];
    glm::mat4 eye_projection[2];
    glm::mat4 eye_offset[2];
    glm::mat4 eye_view[2];
    
    Shader * duplication_shader;
    
    Mesh square_mesh;
    Buffer * square_buffer;
    VertexArray * square_array;
    
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
