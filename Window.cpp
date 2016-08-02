
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

Window::Window()  {
    window = nullptr;
    mouse = nullptr;
    keyboard = nullptr;
    for (unsigned i = 0; i < sizeof(joystick) / sizeof(joystick[0]); ++i)
        joystick[i] = nullptr;
    head = nullptr;
    for (unsigned i = 0; i < sizeof(controller) / sizeof(controller[0]); ++i)
        controller[i] = nullptr;
#ifndef GLOW_NO_OPENVR
    hmd = nullptr;
#endif
}

Window::~Window() {
    delete mouse;
    delete keyboard;
    for (unsigned i = 0; i < sizeof(joystick) / sizeof(joystick[0]); ++i)
        delete joystick[i];
    delete head;
    for (unsigned i = 0; i < sizeof(controller) / sizeof(controller[0]); ++i)
        delete controller[i];
#ifndef GLOW_NO_OPENVR
    if (hmd)
        vr::VR_Shutdown();
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
    
    // Initialize data
    time = 0.0;
    glfwSetTime(0.0);
    dt = 0.01;
    mouse = new Mouse(this);
    keyboard = new Keyboard(this);
    for (unsigned i = 0; i < sizeof(joystick) / sizeof(joystick[0]); ++i)
        joystick[i] = new Joystick(i);
    
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
    head = new Head();
    head->left.setProjection(toGlm(hmd->GetProjectionMatrix(vr::Eye_Left, eye_near, eye_far, vr::API_OpenGL)));
    head->right.setProjection(toGlm(hmd->GetProjectionMatrix(vr::Eye_Right, eye_near, eye_far, vr::API_OpenGL)));
    head->offset[0] = toGlm(hmd->GetEyeToHeadTransform(vr::Eye_Left));
    head->offset[1] = toGlm(hmd->GetEyeToHeadTransform(vr::Eye_Right));
    // TODO why do I have to do this inversion?
    head->offset[0][3].x *= -1;
    head->offset[1][3].x *= -1;
    // TODO compute distortion at some point?
    hmd->GetRecommendedRenderTargetSize(&head->width, &head->height);
    std::cout << "Eye framebuffer has size " << head->width << "x" << head->height << std::endl;
    
    // Create camera framebuffers
    for (unsigned int i = 0; i < 2; ++i) {
        head->texture[i].createColor(head->width, head->height);
        head->framebuffer[i].bind();
        head->framebuffer[i].attach(head->texture[i]);
        head->framebuffer[i].validate();
    }
    head->left.setFramebuffer(&head->framebuffer[0]);
    head->right.setFramebuffer(&head->framebuffer[1]);
    
    // Initialize controllers
    for (unsigned i = 0; i < sizeof(controller) / sizeof(controller[0]); ++i)
        controller[i] = new Controller();
    for (unsigned int i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
        device_type[i] = vr::TrackedDeviceClass_Invalid;
    
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

Mouse const * Window::getMouse() const {
    return mouse;
}

Keyboard const * Window::getKeyboard() const {
    return keyboard;
}

Joystick const * Window::getJoystick(uint32_t index) const {
    return index < sizeof(joystick) / sizeof(joystick[0]) ? joystick[index] : nullptr;
}

Head const * Window::getHead() const {
    return head;
}

Controller const * Window::getController(uint32_t index) const {
    return index < sizeof(controller) / sizeof(controller[0]) ? controller[index] : nullptr;
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
        vr::Texture_t left = {(void*)head->texture[0].getHandle(), vr::API_OpenGL, vr::ColorSpace::ColorSpace_Linear};
        vr::Texture_t right = {(void*)head->texture[1].getHandle(), vr::API_OpenGL, vr::ColorSpace::ColorSpace_Linear};
        compositor->Submit(vr::Eye_Left, &left);
        compositor->Submit(vr::Eye_Right, &right);
        // TODO should we manually flush this? need to check with WaitGetPoses (as for now the camera is jittering)
        //glFlush();
        //vr::VRCompositor()->PostPresentHandoff();
        
        // Render content on screen
        // TODO add option to disable this behaviour
        if (true) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, head->framebuffer[0].getHandle());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, head->width, head->height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
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
                        for (unsigned j = 0; j < sizeof(controller) / sizeof(controller[0]); ++j)
                            if (controller[j]->index < 0)
                                controller[j]->index = i;
                        std::cout << "controller" << std::endl;
                        break;
                    case vr::TrackedDeviceClass_TrackingReference:
                        std::cout << "tracking reference" << std::endl;
                        break;
                    default:
                        for (unsigned j = 0; j < sizeof(controller) / sizeof(controller[0]); ++j)
                            if (controller[j]->index == (int)i)
                                controller[j]->index = -1;
                        std::cout << "invalid" << std::endl;
                }
            }
        }
        
        // Update head and controllers objects
        glm::mat4 t(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        if (device[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
            head->transform = t * toGlm(device[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking);
            head->velocity = glm::vec3(t * glm::vec4(toGlm(device[vr::k_unTrackedDeviceIndex_Hmd].vVelocity), 0.0f));
            // TODO angular velocity
            glm::mat4 matrix = glm::inverse(head->transform);
            head->left.setView(head->offset[0] * matrix);
            head->right.setView(head->offset[1] * matrix);
        }
        for (unsigned i = 0; i < sizeof(controller) / sizeof(controller[0]); ++i)
            if (controller[i]->index >= 0) {
                controller[i]->transform = t * toGlm(device[controller[i]->index].mDeviceToAbsoluteTracking);
                controller[i]->velocity = glm::vec3(t * glm::vec4(toGlm(device[controller[i]->index].vVelocity), 0.0f));
                hmd->GetControllerState(controller[i]->index, &controller[i]->state);
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
    
    // Update gamepads
    mouse->update();
    keyboard->update();
    for (uint32_t i = 0; i < sizeof(joystick) / sizeof(joystick[0]); ++i)
        joystick[i]->update();
    
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
