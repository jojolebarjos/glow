#version 330 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 coordinate;

out vec2 v_coordinateM;
out vec2 v_coordinateNW;
out vec2 v_coordinateNE;
out vec2 v_coordinateSW;
out vec2 v_coordinateSE;

uniform sampler2D texture;

void main() {

    // Transfer basic informations
    gl_Position = vec4(position, 1.0);
    v_coordinateM = coordinate;

    // Precompute some coordinates to optimize dependent texture reads
    vec2 size = textureSize(texture, 0);
    v_coordinateNW = coordinate + vec2(-1.0,  1.0) / size;
    v_coordinateNE = coordinate + vec2( 1.0,  1.0) / size;
    v_coordinateSW = coordinate + vec2(-1.0, -1.0) / size;
    v_coordinateSE = coordinate + vec2( 1.0, -1.0) / size;
}

