#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 coordinate;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_coordinate;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    v_position = (model * vec4(position, 1.0)).xyz;
    v_normal = (transpose(inverse(model)) * vec4(normal, 0.0)).xyz;
    v_coordinate = coordinate;
}
