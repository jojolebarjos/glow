#version 330 core

in vec2 v_coordinate;

out vec4 color;

uniform sampler2D texture;

void main() {
    vec3 original = texture2D(texture, v_coordinate).rgb;
    // TODO tone mapping
    // TODO gamma correction
    color = vec4(original, 1.0);
}
