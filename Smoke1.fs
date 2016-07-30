#version 330 core

in vec2 v_coordinate;
in vec2 v_coordinate_top;
in vec2 v_coordinate_bottom;
in vec2 v_coordinate_left;
in vec2 v_coordinate_right;

out vec4 next;

uniform sampler2D previous;

uniform int mode;
uniform vec2 location;
uniform float radius;
uniform vec2 direction;

void main() {

    // Get neighbourhood
    vec2 size = textureSize(previous, 0);
    vec4 center = texture2D(previous, v_coordinate);
    vec4 top    = texture2D(previous, v_coordinate_top);
    vec4 bottom = texture2D(previous, v_coordinate_bottom);
    vec4 left   = texture2D(previous, v_coordinate_left);
    vec4 right  = texture2D(previous, v_coordinate_right);

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
