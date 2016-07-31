
#include "Window.hpp"
#include "Listener.hpp"

#ifndef GLOW_NO_PNG_ZLIB
#include <png.h>
#endif

#ifndef GLOW_NO_JPEG
extern "C" {
#include <jpeglib.h>
}
#endif

static void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const * message, void const * userParam) {
    std::cout << glGetFriendlyName(source) << ':' << glGetFriendlyName(type) << ':' << glGetFriendlyName(severity) << ' ' << message << std::endl;
}

Window::Window() {
    window = nullptr;
#ifndef GLOW_NO_OPENVR
    hmd = nullptr;
    eye_width = 0;
    eye_height = 0;
    eye_texture[0] = nullptr;
    eye_texture[1] = nullptr;
    eye_framebuffer[0] = nullptr;
    eye_framebuffer[1] = nullptr;
#endif
}

Window::~Window() {
#ifndef GLOW_NO_OPENVR
    if (hmd) {
        vr::VR_Shutdown();
        delete eye_framebuffer[0];
        delete eye_framebuffer[1];
        delete eye_texture[0];
        delete eye_texture[1];
    }
#endif
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}

bool Window::initialize(uint32_t width, uint32_t height, bool stereoscopy, bool debug) {
    // TODO check if GLFW is already initialized
    
    // Print libraries infos
    std::cout << "GLFW: " << glfwGetVersionString() << std::endl;
    std::cout << "GLEW: " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "GLM: " << GLM_VERSION_MAJOR << '.' << GLM_VERSION_MINOR << '.' << GLM_VERSION_PATCH << '.' << GLM_VERSION_REVISION << std::endl;
#ifndef GLOW_NO_JPEG
    std::cout << "libjpeg: " << (JPEG_LIB_VERSION / 10) << (char)(JPEG_LIB_VERSION % 10 + 'a' - 1) << std::endl;
#else
    std::cout << "libjpeg: <none>" << std::endl;
#endif
#ifndef GLOW_PNG_NO_ZLIB
    std::cout << "libpng: " << PNG_LIBPNG_VER_MAJOR << '.' << PNG_LIBPNG_VER_MINOR << '.' << PNG_LIBPNG_VER_RELEASE << std::endl;
#else
    std::cout << "libpng: <none>" << std::endl;
#endif
    std::cout << "Bullet: " << (BT_BULLET_VERSION / 100) << '.' << ((BT_BULLET_VERSION / 10) % 10) << '.' << (BT_BULLET_VERSION % 10) << std::endl;
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Create window
    if (debug)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
    glfwWindowHint(GLFW_RESIZABLE, 0);
    window = glfwCreateWindow(width, height, "Glow", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        std::cout << "Failed to create GLFW window" << std::endl;
        return false;
    }
    glfwMakeContextCurrent(window);
    
    // Load GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(window);
        window = nullptr;
        glfwTerminate();
        std::cout << "Failed to initialize GLEW" << std::endl;
        return false;
    }
    
    // Enable OpenGL debugging
    if (debug) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(debugCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }
    
    // Get default framebuffer size
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    this->width = w;
    this->height = h;
    
    // Print OpenGL infos
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Screen framebuffer has size " << w << "x" << h << std::endl;
    
    // Initialize time
    time = 0.0;
    glfwSetTime(0.0);
    dt = 0.01;
    
    // Initialize mouse and keyboard
    current = 0;
    for (int i = 0; i <= GLFW_MOUSE_BUTTON_LAST; ++i) {
        mouse_button[0][i] = GLFW_RELEASE;
        mouse_button[1][i] = GLFW_RELEASE;
    }
    for (int i = 0; i <= GLFW_KEY_LAST; ++i) {
        keyboard_button[0][i] = GLFW_RELEASE;
        keyboard_button[1][i] = GLFW_RELEASE;
    }
    
    // Initialize gamepads
    for (uint32_t i = 0; i < sizeof(gamepad) / sizeof(Gamepad); ++i) {
        gamepad[i].connected = false;
        gamepad[i].axis_count = 0;
        gamepad[i].button_count = 0;
    }
    
#ifndef GLOW_NO_OPENVR
    
    // Check VR capabilities
    if (!stereoscopy)
        return true;
    if (!vr::VR_IsHmdPresent()) {
        std::cout << "No head mounted display present" << std::endl;
        return true;
    }
    if (!vr::VR_IsRuntimeInstalled()) {
        std::cout << "Virtual reality runtime not installed" << std::endl;
        return true;
    }
    
     // Create HDM system
    vr::HmdError error;
    hmd = VR_Init(&error, vr::VRApplication_Scene);
    if (error != vr::VRInitError_None) {
        vr::VR_Shutdown();
        hmd = nullptr;
        std::cout << "Failed to initialize HDM system (" << VR_GetVRInitErrorAsSymbol(error) << ")" << std::endl;
        return true;
    }
    
    // Check compositor existence
    vr::IVRCompositor * compositor = vr::VRCompositor();
    if (!compositor) {
        vr::VR_Shutdown();
        hmd = nullptr;
        std::cout << "Failed to create compositor" << std::endl;
        return true;
    }
    
    // Print infos
    char buffer[256] = {0};
    hmd->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String, buffer, sizeof(buffer), nullptr);
    std::cout << "Virtual reality tracking system: " << buffer << std::endl;
    hmd->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_ManufacturerName_String, buffer, sizeof(buffer), nullptr);
    std::cout << "Virtual reality manufacturer: " << buffer << std::endl;
    
    // Get camera properties
    // TODO provide these near/far somewhere
    float eye_near = 0.01f;
    float eye_far = 1000.0f;
    eye_projection[0] = toGlm(hmd->GetProjectionMatrix(vr::Eye_Left, eye_near, eye_far, vr::API_OpenGL));
    eye_projection[1] = toGlm(hmd->GetProjectionMatrix(vr::Eye_Right, eye_near, eye_far, vr::API_OpenGL));
    eye_offset[0] = toGlm(hmd->GetEyeToHeadTransform(vr::Eye_Left));
    eye_offset[1] = toGlm(hmd->GetEyeToHeadTransform(vr::Eye_Right));
    // TODO why do I have to do this inversion?
    eye_offset[0][3].x *= -1;
    eye_offset[1][3].x *= -1;
    // TODO compute distortion at some point?
    hmd->GetRecommendedRenderTargetSize(&eye_width, &eye_height);
    std::cout << "Eye framebuffer has size " << eye_width << "x" << eye_height << std::endl;
    for (unsigned int i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
        device_type[i] = vr::TrackedDeviceClass_Invalid;
    
    // Create framebuffers
    for (unsigned int i = 0; i < 2; ++i) {
        eye_texture[i] = new Texture();
        eye_texture[i]->createColor(eye_width, eye_height);
        eye_framebuffer[i] = new Framebuffer();
        eye_framebuffer[i]->bind();
        eye_framebuffer[i]->attach(*eye_texture[i]);
        eye_framebuffer[i]->validate();
    }
    
#endif
    return true;
}

GLFWwindow * Window::getHandle() const {
    return window;
}

uint32_t Window::getWidth() const {
    return width;
}

uint32_t Window::getHeight() const {
    return height;
}

float Window::getTime() const {
    return time;
}

float Window::getDeltaTime() const {
    return dt;
}

bool Window::hasFocus() const {
    return glfwGetWindowAttrib(window, GLFW_FOCUSED);
}

bool Window::isMouseButtonDown(uint32_t index) const {
    return index <= GLFW_MOUSE_BUTTON_LAST ? mouse_button[current][index] == GLFW_PRESS : false;
}

bool Window::isMouseButtonPressed(uint32_t index) const {
    return index <= GLFW_MOUSE_BUTTON_LAST ? mouse_button[current ^ 1][index] == GLFW_RELEASE && mouse_button[current][index] == GLFW_PRESS : false;
}

bool Window::isMouseButtonReleased(uint32_t index) const {
    return index <= GLFW_MOUSE_BUTTON_LAST ? mouse_button[current ^ 1][index] == GLFW_PRESS && mouse_button[current][index] == GLFW_RELEASE : false;
}

glm::vec2 Window::getMouseLocation() const {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return {x, height - y};
}

bool Window::isKeyboardButtonDown(uint32_t index) const {
    return index <= GLFW_KEY_LAST ? keyboard_button[current][index] == GLFW_PRESS : false;
}

bool Window::isKeyboardButtonPressed(uint32_t index) const {
    return index <= GLFW_KEY_LAST ? keyboard_button[current ^ 1][index] == GLFW_RELEASE && keyboard_button[current][index] == GLFW_PRESS : false;
}

bool Window::isKeyboardButtonReleased(uint32_t index) const {
    return index <= GLFW_KEY_LAST ? keyboard_button[current ^ 1][index] == GLFW_PRESS && keyboard_button[current][index] == GLFW_RELEASE : false;
}

bool Window::isGamepadConnected(uint32_t index) const {
    return index < sizeof(gamepad) / sizeof(Gamepad) ? gamepad[index].connected : false;
}

uint32_t Window::getGamepadAxisCount(uint32_t index) const {
    return index < sizeof(gamepad) / sizeof(Gamepad) ? gamepad[index].axis_count : 0;
}

float Window::getGamepadAxis(uint32_t index, uint32_t axis_index) const {
    return index < sizeof(gamepad) / sizeof(Gamepad) && axis_index < gamepad[index].axis_count ? gamepad[index].axis[axis_index] : 0.0f;
}

uint32_t Window::getGamepadButtonCount(uint32_t index) const {
    return index < sizeof(gamepad) / sizeof(Gamepad) ? gamepad[index].button_count : 0;
}

bool Window::isGamepadButtonDown(uint32_t index, uint32_t button_index) const {
    return index < sizeof(gamepad) / sizeof(Gamepad) && button_index < gamepad[index].button_count ? gamepad[index].button[button_index] : false;
}

bool Window::hasStereoscopy() const {
#ifndef GLOW_NO_OPENVR
    return hmd;
#else
    return false;
#endif
}

uint32_t Window::getEyeWidth() const {
#ifndef GLOW_NO_OPENVR
    return eye_width;
#else
    return 0;
#endif
}

uint32_t Window::getEyeHeight() const {
#ifndef GLOW_NO_OPENVR
    return eye_height;
#else
    return 0;
#endif
}

Framebuffer * Window::getEyeFramebuffer(unsigned int index) {
#ifndef GLOW_NO_OPENVR
    if (index < 2)
        return eye_framebuffer[index];
#endif
    return nullptr;
}

glm::mat4 Window::getEyeProjection(unsigned int index) const {
#ifndef GLOW_NO_OPENVR
    if (index < 2)
        return eye_projection[index];
#endif
    return glm::mat4();
}

glm::mat4 Window::getEyeView(unsigned int index) const {
#ifndef GLOW_NO_OPENVR
    if (index < 2)
        return eye_view[index];
#endif
    return glm::mat4();    
}

bool Window::isDeviceConnected(uint32_t index) const {
#ifndef GLOW_NO_OPENVR
    if (index < vr::k_unMaxTrackedDeviceCount)
        return device_type[index] != vr::TrackedDeviceClass_Invalid;
#endif
    return false;
}

bool Window::isDeviceHead(uint32_t index) const {
#ifndef GLOW_NO_OPENVR
    if (index < vr::k_unMaxTrackedDeviceCount)
        return device_type[index] == vr::TrackedDeviceClass_HMD;
#endif
    return false;    
}

bool Window::isDeviceController(uint32_t index) const {
#ifndef GLOW_NO_OPENVR
    if (index < vr::k_unMaxTrackedDeviceCount)
        return device_type[index] == vr::TrackedDeviceClass_Controller;
#endif
    return false;    
}

bool Window::isDeviceReference(uint32_t index) const {
#ifndef GLOW_NO_OPENVR
    if (index < vr::k_unMaxTrackedDeviceCount)
        return device_type[index] == vr::TrackedDeviceClass_TrackingReference;
#endif
    return false;    
}

uint32_t Window::getDeviceHead() const {
#ifndef GLOW_NO_OPENVR
    return vr::k_unTrackedDeviceIndex_Hmd;
#else
    return ~(uint32_t)0;
#endif
}

uint32_t Window::getDeviceController(uint32_t id) const {
#ifndef GLOW_NO_OPENVR
    for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
        if (device_type[i] == vr::TrackedDeviceClass_Controller) {
            if (id == 0)
                return i;
            --id;
        }
#endif
    return ~(uint32_t)0;
}
uint32_t Window::getDeviceReference(uint32_t id) const {
    #ifndef GLOW_NO_OPENVR
    for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
        if (device_type[i] == vr::TrackedDeviceClass_TrackingReference) {
            if (id == 0)
                return i;
            --id;
        }
#endif
    return ~(uint32_t)0;
}

glm::mat4 Window::getDeviceTransform(uint32_t index) const {
#ifndef GLOW_NO_OPENVR
    if (index < vr::k_unMaxTrackedDeviceCount)
        return device_transform[index];
#endif
    return glm::mat4();    
}

glm::vec3 Window::getDeviceVelocity(uint32_t index) const {
#ifndef GLOW_NO_OPENVR
    if (index < vr::k_unMaxTrackedDeviceCount)
        return device_velocity[index];
#endif
    return glm::vec3();
}

glm::vec3 Window::getDeviceAngularVelocity(uint32_t index) const {
#ifndef GLOW_NO_OPENVR
    if (index < vr::k_unMaxTrackedDeviceCount)
        return device_angularVelocity[index];
#endif
    return glm::vec3();
}

bool Window::isDeviceButtonDown(uint32_t index, uint32_t button_index) const {
#ifndef GLOW_NO_OPENVR
    if (isDeviceController(index) && button_index < 64)
        return (state[index].ulButtonPressed & vr::ButtonMaskFromId((vr::EVRButtonId)button_index)) != 0;
#endif
    return false;
}

bool Window::update() {
    
    // Check if running
    if (!window)
        return false;
    
    // Update VR
#ifndef GLOW_NO_OPENVR
    if (hmd) {
        vr::IVRCompositor * compositor = vr::VRCompositor();
        
        // Submit texture for this frame
        vr::Texture_t left = {(void*)eye_texture[0]->getHandle(), vr::API_OpenGL, vr::ColorSpace::ColorSpace_Linear};
        vr::Texture_t right = {(void*)eye_texture[1]->getHandle(), vr::API_OpenGL, vr::ColorSpace::ColorSpace_Linear};
        compositor->Submit(vr::Eye_Left, &left);
        compositor->Submit(vr::Eye_Right, &right);
        // TODO should we manually flush this? need to check with WaitGetPoses (as for now the camera is jittering)
        //glFlush();
        //vr::VRCompositor()->PostPresentHandoff();
        
        // Render content on screen
        // TODO add option to disable this behaviour
        if (true) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, eye_framebuffer[0]->getHandle());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, eye_width, eye_height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        }
        
        // Update tracked device for next frame
        // TODO WaitGetPoses is equivalent to vsync:
        // - should disable screen vsync
        // - should start to render new frame before that
        compositor->WaitGetPoses(device, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
        for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i) {
            vr::ETrackedDeviceClass type = hmd->GetTrackedDeviceClass(i);
            if (type != device_type[i]) {
                device_type[i] = type;
                std::cout << "Device " << i << " is ";
                switch (type) {
                    case vr::TrackedDeviceClass_HMD:
                        std::cout << "head" << std::endl;
                        break;
                    case vr::TrackedDeviceClass_Controller:
                        std::cout << "controller" << std::endl;
                        break;
                    case vr::TrackedDeviceClass_TrackingReference:
                        std::cout << "tracking reference" << std::endl;
                        break;
                    default:
                        std::cout << "invalid" << std::endl;
                }
            }
            if (device[i].bPoseIsValid) {
                // TODO is this transform correct?
                glm::mat4 t(1, 0, 0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1);
                device_transform[i] =  t * toGlm(device[i].mDeviceToAbsoluteTracking);
                // TODO fix axes of velocities
                device_velocity[i] = toGlm(device[i].vVelocity);
                device_angularVelocity[i] = toGlm(device[i].vAngularVelocity);
            }
            if (type == vr::TrackedDeviceClass_Controller) {
                hmd->GetControllerState(i, &state[i]);
                //std::cout << state[i].ulButtonPressed << " " << vr::ButtonMaskFromId((vr::EVRButtonId)33) << std::endl;
            }
        }
        
        // Update view matrices
        if (device[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
            glm::mat4 matrix = glm::inverse(device_transform[vr::k_unTrackedDeviceIndex_Hmd]);
            eye_view[0] = eye_offset[0] * matrix;
            eye_view[1] = eye_offset[1] * matrix;
        }
        
        // TODO poll events?
        // https://github.com/ValveSoftware/openvr/wiki/IVRSystem::PollNextEvent
        // https://github.com/ValveSoftware/openvr/wiki/VREvent_t
        // also useful for GLFW? or is active polling good enough?
        
    }    
#endif
    
    // Update input and buffers
    glfwPollEvents();
    glfwSwapBuffers(window);
    
    // Update mouse and keyboard
    current ^= 1;
    for (int i = 0; i <= GLFW_MOUSE_BUTTON_LAST; ++i)
        mouse_button[current][i] = glfwGetMouseButton(window, i);
    for (int i = 0; i <= GLFW_KEY_LAST; ++i)
        keyboard_button[current][i] = glfwGetKey(window, i);
    
    // Update gamepads
    for (uint32_t i = 0; i < sizeof(gamepad) / sizeof(Gamepad); ++i) {
        bool connected = glfwJoystickPresent(GLFW_JOYSTICK_1 + i);
        if (connected) {
            if (!gamepad[i].connected)
                std::cout << "Gamepad " << i << " connected" << std::endl;
            int count;
            float const * axis = glfwGetJoystickAxes(i, &count);
            gamepad[i].axis_count = glm::max((unsigned)count, sizeof(gamepad[i].axis) / sizeof(gamepad[i].axis[0]));
            for (uint32_t j = 0; j < gamepad[i].axis_count; ++j)
                gamepad[i].axis[j] = axis[j];
            unsigned char const * button = glfwGetJoystickButtons(i, &count);
            gamepad[i].button_count = glm::max((unsigned)count, sizeof(gamepad[i].button) / sizeof(gamepad[i].button[0]));
            for (uint32_t j = 0; j < gamepad[i].button_count; ++j)
                gamepad[i].button[j] = button[j] == GLFW_PRESS;
        } else if (gamepad[i].connected)
            std::cout << "Gamepad " << i << " disconnected" << std::endl;
        gamepad[i].connected = connected;
    }
    
    // Update time
    double now = glfwGetTime();
    dt = now - time;
    time = now;
    
    // Check if user want to quit
    return !glfwWindowShouldClose(window);
}

#ifndef GLOW_NO_OPENVR

glm::mat4 Window::toGlm(vr::HmdMatrix34_t const & matrix) const {
    return glm::mat4(
        matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], 0.0f,
        matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], 0.0f,
        matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], 0.0f,
        matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], 1.0f
    );
}

glm::mat4 Window::toGlm(vr::HmdMatrix44_t const & matrix) const {
    return glm::mat4(
        matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], matrix.m[3][0],
        matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], matrix.m[3][1],
        matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], matrix.m[3][2],
        matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], matrix.m[3][3]
    );
}

glm::vec3 Window::toGlm(vr::HmdVector3_t const & vector) const {
    return glm::vec3(vector.v[0], vector.v[1], vector.v[2]);
}

#endif
