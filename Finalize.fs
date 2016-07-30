#version 330 core

in vec2 v_coordinate;

out vec4 color;

uniform sampler2D texture_color;
uniform sampler2D texture_position;
uniform sampler2D texture_normal;
uniform sampler2D texture_light;

void main() {
    color = texture2D(texture_light, v_coordinate) * texture2D(texture_color, v_coordinate);
}
