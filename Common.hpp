
#ifndef GLOW_COMMON_HPP
#define GLOW_COMMON_HPP

#define GLOW_DEBUG_CONTEXT
#define GLOW_PNG_ZLIB
#define GLOW_JPEG
#define GLOW_OPENAL
#define GLOW_OGG_VORBIS
#define GLOW_OPENVR

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#ifdef GLOW_OPENAL
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/efx.h>
#endif

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#include <cmath>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>

#undef PI
#define PI 3.1415926535897932384626433832795f

template <typename T>
std::ostream & operator<<(std::ostream & out, glm::tvec2<T> const & v) {
    return out << v.x << ',' << v.y;
}

template <typename T>
std::ostream & operator<<(std::ostream & out, glm::tvec3<T> const & v) {
    return out << v.x << ',' << v.y << ',' << v.z;
}

template <typename T>
std::ostream & operator<<(std::ostream & out, glm::tvec4<T> const & v) {
    return out << v.x << ',' << v.y << ',' << v.z << ',' << v.w;
}

template <typename T>
std::ostream & operator<<(std::ostream & out, glm::tmat4x4<T> const & m) {
    return out << m[0][0] << ',' << m[1][0] << ',' << m[2][0] << ',' << m[3][0] << std::endl
               << m[0][1] << ',' << m[1][1] << ',' << m[2][1] << ',' << m[3][1] << std::endl
               << m[0][2] << ',' << m[1][2] << ',' << m[2][2] << ',' << m[3][2] << std::endl
               << m[0][3] << ',' << m[1][3] << ',' << m[2][3] << ',' << m[3][3] << std::endl;
}

inline char const * glGetFriendlyName(GLenum name) {
    switch (name) {
        
        // Debug message sources
        case GL_DEBUG_SOURCE_API:
            return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "window system";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "source shader compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "third party";
        case GL_DEBUG_SOURCE_APPLICATION:
            return "application";
        case GL_DEBUG_SOURCE_OTHER:
            return "other";
            
        // Debug message types
        case GL_DEBUG_TYPE_ERROR:
            return "error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "deprecated behavior";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "undefined behavior";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "portability";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "performance";
        case GL_DEBUG_TYPE_MARKER:
            return "marker";
        case GL_DEBUG_TYPE_PUSH_GROUP:
            return "push";
        case GL_DEBUG_TYPE_POP_GROUP:
            return "pop";
        case GL_DEBUG_TYPE_OTHER:
            return "other";
            
        // Debug message severity
        case GL_DEBUG_SEVERITY_HIGH:
            return "high";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "medium";
        case GL_DEBUG_SEVERITY_LOW:
            return "low";
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "notification";
            
    }
    return "<unknown>";
}

inline btVector3 fromGLM(glm::vec3 const & v) {
    return btVector3(v.x, v.y, v.z);
}

inline glm::vec3 toGLM(btVector3 const & v) {
    return glm::vec3(v.x(), v.y(), v.z());
}

#endif
