#version 330 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 coordinate;

out vec2 v_coordinate;
out vec2 v_coordinate_top;
out vec2 v_coordinate_bottom;
out vec2 v_coordinate_left;
out vec2 v_coordinate_right;

uniform sampler2D previous;

void main() {

    // Transfer basic informations
    gl_Position = vec4(position, 1.0);
    v_coordinate = coordinate;

    // Precompute some coordinates to optimize dependent texture reads
    vec2 size = textureSize(previous, 0);
    v_coordinate_top    = coordinate + vec2( 0.0,  1.0) / size;
    v_coordinate_bottom = coordinate + vec2( 0.0, -1.0) / size;
    v_coordinate_left   = coordinate + vec2(-1.0,  0.0) / size;
    v_coordinate_right  = coordinate + vec2( 1.0,  0.0) / size;
}
