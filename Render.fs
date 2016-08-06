#version 330 core

in vec3 v_position;
in vec3 v_normal;
in vec2 v_coordinate;
in vec4 v_extra;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 position;
layout(location = 2) out vec4 normal;
layout(location = 3) out vec4 light;

uniform sampler2DArray textures;

void main() {
    color = texture(textures, vec3(v_coordinate, v_extra.x));
    position = vec4(v_position, 1.0);
    normal = vec4(v_normal, 0.0);
    light = vec4(0.0, 0.0, 0.0, 1.0);
}
