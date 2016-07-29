#version 330 core

in vec2 v_coordinate;

out vec4 next;

uniform sampler2D previous;

uniform int mode;
uniform vec2 location;
uniform float radius;
uniform vec2 direction;

void main() {

    // Get neighbourhood
    vec2 size = textureSize(previous, 0);
    vec4 center = texture2D(previous, gl_FragCoord.xy / size);
    vec4 top    = texture2D(previous, (gl_FragCoord.xy + vec2( 0.0,  1.0)) / size);
    vec4 bottom = texture2D(previous, (gl_FragCoord.xy + vec2( 0.0, -1.0)) / size);
    vec4 left   = texture2D(previous, (gl_FragCoord.xy + vec2(-1.0,  0.0)) / size);
    vec4 right  = texture2D(previous, (gl_FragCoord.xy + vec2( 1.0,  0.0)) / size);

    // Compute advection effect
    vec2 advection = texture2D(previous, (gl_FragCoord.xy - center.xy) / size).xy;

    // Compute pressure effect
    vec2 pressure = vec2(left.z - right.z, bottom.z - top.z) / 2.0;

    // Compute force effect
    vec2 force = vec2(0.0, 0.0);
    if (mode > 0) {
        float distance = length(gl_FragCoord.xy - location);
        if (distance <= radius) {
            if (mode == 1)
                force = direction;
            else if (mode == 2)
                center.w = 2.0;
        }
    }

    // Save result
    next = vec4(advection + pressure + force, center.zw);
}
