#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 coordinate;

out vec3 v_normal;
out vec2 v_coordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
	v_normal = normal;
    v_coordinate = coordinate;
}