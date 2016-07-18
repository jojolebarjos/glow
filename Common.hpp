
#ifndef GLOW_COMMON_HPP
#define GLOW_COMMON_HPP

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>

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

#endif
