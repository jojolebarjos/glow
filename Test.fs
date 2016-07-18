#version 330 core

in vec2 v_coordinate;

out vec4 color;

uniform sampler2D tex;

void main() {
    //color = vec4(v_coordinate, 1.0);
    color = texture2D(tex, v_coordinate);
}