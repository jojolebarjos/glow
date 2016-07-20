#version 330 core

in vec2 v_coordinate;

out vec4 color;

uniform vec2 size;
uniform sampler2D previous;

uniform int mode;

void main() {
    vec4 center = texture2D(previous, gl_FragCoord.xy / size);
    if (mode == 0)
        color = vec4(center.xyw, 1.0);
    else
        color = vec4(center.w * + (1.0 + center.x * 0.1), center.w * + (1.0 + center.y * 0.1), center.w * + (1.0 + length(center.xy)), 1.0);
}
