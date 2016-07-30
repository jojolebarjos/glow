#version 330 core

in vec2 v_coordinate;

out vec4 color;

uniform vec3 light_position;
uniform vec3 light_color;
uniform float light_radius;

uniform sampler2D texture_position;
uniform sampler2D texture_normal;

void main() {

    // Get geometry properties
    vec3 position = texture2D(texture_position, v_coordinate).xyz;
    vec3 normal = texture2D(texture_normal, v_coordinate).xyz;
    vec3 delta = light_position - position;
    float distance = length(delta);
    
    // Fragment squared distance
    float distance_factor = max(distance / light_radius, 0.0);
    distance_factor = 1.0 - distance_factor * distance_factor;

    // Fragment orientation
    float exposition = dot(delta, normal) / (distance * length(normal));
    float exposition_factor = max(exposition, 0.0);

    // Combine factors to produce final illumination
    float factor = distance_factor * exposition_factor;
    color = vec4(light_color * factor, 1.0);
}
