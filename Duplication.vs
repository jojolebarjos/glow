#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 coordinate;

out vec2 v_coordinate;

uniform float offset;

void main() {
    gl_Position = vec4(position.x * 0.49 + offset, position.y, 0.0, 1.0);
    v_coordinate = coordinate;
}
