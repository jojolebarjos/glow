#version 330 core

in vec3 v_position;
in vec3 v_normal;

out vec4 color;

uniform vec3 light_position;
uniform vec3 light_color;
uniform float light_radius;

void main() {
    vec3 delta = light_position - v_position;
    float distance = length(delta);
    
    // Fragment squared distance
    float distance_factor = max(distance / light_radius, 0.0);
    distance_factor = 1.0 - distance_factor * distance_factor;

    // Fragment orientation
    float exposition = dot(delta, v_normal) / (distance * length(v_normal));
    float exposition_factor = max(exposition, 0.0);

    // Combine factors to produce final illumination
    float factor = distance_factor * exposition_factor;
    color = vec4(light_color * factor, 1.0);
}
