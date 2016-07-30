#version 330 core

in vec2 v_coordinate;

out vec4 color;

uniform sampler2D texture;

void main() {

    // Get resolution
    vec2 size = textureSize(texture, 0);
    vec2 delta = vec2(1.0, 1.0) / size;

    // Get neighbourhood
    vec3 colorC = texture2D(texture, v_coordinate).rgb;
    vec3 colorMM = texture2D(texture, v_coordinate + vec2(-delta.x, -delta.y)).rgb;
    vec3 colorPM = texture2D(texture, v_coordinate + vec2( delta.x, -delta.y)).rgb;
    vec3 colorMP = texture2D(texture, v_coordinate + vec2(-delta.x,  delta.y)).rgb;
    vec3 colorPP = texture2D(texture, v_coordinate + vec2( delta.x,  delta.y)).rgb;

    // Compute luminance
    vec3 luminance = vec3(0.299, 0.587, 0.114);
    float luminanceC = dot(colorC, luminance);
    float luminanceMM = dot(colorMM, luminance);
    float luminancePM = dot(colorPM, luminance);
    float luminanceMP = dot(colorMP, luminance);
    float luminancePP = dot(colorPP, luminance);

    // Get minimal and maximal luminance
    float luminanceMin = min(min(min(luminanceMM, luminanceMP), min(luminancePM, luminancePP)), luminanceC);
    float luminanceMax = max(max(max(luminanceMM, luminanceMP), max(luminancePM, luminancePP)), luminanceC);

    // Compute edge direction
    vec2 direction = vec2(
        (luminanceMP + luminancePP) - (luminanceMM + luminancePM),
        (luminanceMM + luminanceMP) - (luminancePM + luminancePP)
    );

    // Reduce direction according to luminance
    float reduce = max((luminanceMM + luminancePM + luminanceMP + luminancePP) * 0.25 * 0.125, 1.0 / 128.0);
    float coefficient = 1.0 / (min(abs(direction.x), abs(direction.y)) + reduce);
    direction = clamp(direction * coefficient, -8.0, 8.0) * delta;

    // Compute sampling alternatives
    vec3 colorA = 0.5 * (
        texture2D(texture, v_coordinate + direction * (1.0 / 3.0 - 0.5)).rgb +
        texture2D(texture, v_coordinate + direction * (2.0 / 3.0 - 0.5)).rgb
    );
    vec3 colorB = 0.5 * (
        texture2D(texture, v_coordinate + direction * (0.0 / 3.0 - 0.5)).rgb +
        texture2D(texture, v_coordinate + direction * (3.0 / 3.0 - 0.5)).rgb
    );

    // Select best alternative
    float luminanceB = dot(colorB, luminance);
    if (luminanceB < luminanceMin || luminanceB > luminanceMax)
        color = vec4(colorA, 1.0);
    else
        color = vec4(colorB, 1.0);
}
