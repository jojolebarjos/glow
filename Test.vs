#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 coordinate;

out vec2 v_coordinate;

void main() {
    gl_Position = vec4(position, 1.0);
    v_coordinate = coordinate;
}
