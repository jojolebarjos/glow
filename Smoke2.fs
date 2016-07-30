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

    // Compute new pressure (this shader is called until pressure convergence)
    float pressure = (right.z + left.z + top.z + bottom.z - (right.x - left.x + top.y - bottom.y) / 2.0) / 4.0;

    // Save result
    next = vec4(center.xy, pressure, center.w);
}
