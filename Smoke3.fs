#version 330 core

in vec2 v_coordinate;

out vec4 next;

uniform sampler2D previous;

void main() {

    // Get neighbourhood
    vec2 size = textureSize(previous, 0);
    vec4 center = texture2D(previous, gl_FragCoord.xy / size);
    vec4 top    = texture2D(previous, (gl_FragCoord.xy + vec2( 0.0,  1.0)) / size);
    vec4 bottom = texture2D(previous, (gl_FragCoord.xy + vec2( 0.0, -1.0)) / size);
    vec4 left   = texture2D(previous, (gl_FragCoord.xy + vec2(-1.0,  0.0)) / size);
    vec4 right  = texture2D(previous, (gl_FragCoord.xy + vec2( 1.0,  0.0)) / size);

    // Fix velocity using pressure to guarantee fluid incompressibility
    vec2 velocity = center.xy - vec2(right.z - left.z, top.z - bottom.z) / 2.0;

    // Advect ink
    float ink = texture2D(previous, (gl_FragCoord.xy - velocity) / size).w;

    // Save result
    next = vec4(velocity, center.z, ink);
}
