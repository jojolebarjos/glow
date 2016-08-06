#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 coordinate;
layout(location = 3) in mat4 model;
layout(location = 7) in vec4 extra;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_coordinate;
out vec4 v_extra;

uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    v_position = (model * vec4(position, 1.0)).xyz;
    v_normal = (transpose(inverse(model)) * vec4(normal, 0.0)).xyz;
    v_coordinate = coordinate;
    v_extra = extra;
}
