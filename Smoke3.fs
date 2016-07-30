#version 330 core

in vec2 v_coordinate;
in vec2 v_coordinate_top;
in vec2 v_coordinate_bottom;
in vec2 v_coordinate_left;
in vec2 v_coordinate_right;

out vec4 next;

uniform sampler2D previous;

void main() {

    // Get neighbourhood
    vec2 size = textureSize(previous, 0);
    vec4 center = texture2D(previous, v_coordinate);
    vec4 top    = texture2D(previous, v_coordinate_top);
    vec4 bottom = texture2D(previous, v_coordinate_bottom);
    vec4 left   = texture2D(previous, v_coordinate_left);
    vec4 right  = texture2D(previous, v_coordinate_right);

    // Fix velocity using pressure to guarantee fluid incompressibility
    vec2 velocity = center.xy - vec2(right.z - left.z, top.z - bottom.z) / 2.0;

    // Advect ink
    float ink = texture2D(previous, (gl_FragCoord.xy - velocity) / size).w;

    // Save result
    next = vec4(velocity, center.z, ink);
}
