#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 v_position;
out vec3 v_normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    v_position = (model * vec4(position, 1.0)).xyz;
    // TODO correct? should it be the matrix inverse for the normals?
    v_normal = (transpose(inverse(model)) * vec4(normal, 0.0)).xyz;
}
