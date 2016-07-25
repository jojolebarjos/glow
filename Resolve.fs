#version 330 core

in vec2 v_coordinate;

out vec4 color;

uniform sampler2DMS texture;

void main() {
    ivec2 coordinate = ivec2(textureSize(texture) * v_coordinate);
    vec4 sum = vec4(0.0);
    // TODO provide multisampling sample count through uniform?
    for (int i = 0; i < 4; ++i)
        sum += texelFetch(texture, coordinate, i);
    color = sum / 4.0;
}
