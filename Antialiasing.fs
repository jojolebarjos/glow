#version 330 core

in vec2 v_coordinateM;
in vec2 v_coordinateNW;
in vec2 v_coordinateNE;
in vec2 v_coordinateSW;
in vec2 v_coordinateSE;

out vec4 color;

uniform sampler2D texture;

void main() {
    vec2 size = textureSize(texture, 0);

    // Get neighbourhood
    vec3 rgbM  = texture2D(texture, v_coordinateM ).rgb;
    vec3 rgbNW = texture2D(texture, v_coordinateNW).rgb;
    vec3 rgbNE = texture2D(texture, v_coordinateNE).rgb;
    vec3 rgbSW = texture2D(texture, v_coordinateSW).rgb;
    vec3 rgbSE = texture2D(texture, v_coordinateSE).rgb;

    // TODO implement FXAA
    color = vec4((rgbM + rgbNW + rgbNE + rgbSW + rgbSE) * 0.2, 1.0);
}
