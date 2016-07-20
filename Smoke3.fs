#version 330 core

in vec2 v_coordinate;

out vec4 next;

uniform vec2 size;
uniform sampler2D previous;

vec4 decode(sampler2D tex, vec2 delta) {
    vec4 value = texture2D(tex, (gl_FragCoord.xy + delta) / size);
    value -= vec4(0.5, 0.5, 0.5, 0.5);
    value *= 2.0;
    value.z *= 4.0;
    return value;
}

vec4 encode(vec4 value) {
    vec4 data = value;
    data.z /= 4.0;
    data /= 2.0;
    data += vec4(0.5, 0.5, 0.5, 0.5);
    return data;
}

void main() {
    vec4 center = decode(previous, vec2( 0.0,  0.0));
    vec4 top    = decode(previous, vec2( 0.0,  1.0));
    vec4 bottom = decode(previous, vec2( 0.0, -1.0));
    vec4 left   = decode(previous, vec2(-1.0,  0.0));
    vec4 right  = decode(previous, vec2( 1.0,  0.0));
    vec2 velocity = center.xy - vec2(right.z - left.z, top.z - bottom.z) / 2.0;
    float ink = decode(previous, -velocity).w;
    next = encode(vec4(velocity, center.z, ink));
}
