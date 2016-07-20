#version 330 core

in vec2 v_coordinate;

out vec4 next;

uniform vec2 size;
uniform sampler2D previous;

uniform int mode;
uniform vec2 location;
uniform float radius;

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
    vec4 center = decode(previous, vec2(0.0, 0.0));
    vec2 advection = decode(previous, -center.xy).xy;
    vec2 pressure = vec2(
        decode(previous, vec2(-1.0, 0.0)).z - decode(previous, vec2(1.0, 0.0)).z,
        decode(previous, vec2(0.0, -1.0)).z - decode(previous, vec2(0.0, 1.0)).z
    ) / 2.0;
    vec2 force = vec2(0.0, 0.0);
    if (mode > 0) {
        float distance = length(gl_FragCoord.xy - location);
        if (distance <= radius) {
            if (mode == 1)
                force = vec2(0.5, 0.5);
            else if (mode == 2)
                center.w = 2.0;
        }
    }
    next = encode(vec4(advection + pressure + force, center.zw));
}
