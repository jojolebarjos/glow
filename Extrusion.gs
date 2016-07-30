#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 14) out;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 light_position;
uniform float light_radius;

void emit(vec3 p) {

    // Project vertex
    vec4 pos = projection * view * vec4(p, 1.0);

    // Add bias to avoid z-fight
    pos.z += 0.00001 * pos.w;

    // Emit final vertex
    gl_Position = pos;
    EmitVertex();
}

void main() {
    
    // Get triangle coordinates
    vec3 a = gl_in[0].gl_Position.xyz;
    vec3 b = gl_in[1].gl_Position.xyz;
    vec3 c = gl_in[2].gl_Position.xyz;

    // Compute center of mass
    vec3 d = (a + b + c) / 3.0;

    // Compute normal and check orientation w.r.t. the light
    vec3 n = cross(b - a, c - a);
    if (dot(n, d - light_position) >= 0.0)
        return;

    // Extrude vertices w.r.t light position
    vec3 ap = a + normalize(a - light_position) * (2.0 * light_radius);
    vec3 bp = b + normalize(b - light_position) * (2.0 * light_radius);
    vec3 cp = c + normalize(c - light_position) * (2.0 * light_radius);
    vec3 dp = d + normalize(d - light_position) * (3.0 * light_radius);
    
    // TODO is it possible to do this using only one patch?

    // Emit first patch
    // c b cp bp dp ap cp a c b
    emit(c);
    emit(b);
    emit(cp);
    emit(bp);
    emit(dp);
    emit(ap);
    emit(cp);
    emit(a);
    emit(c);
    emit(b);
    EndPrimitive();

    // Emit second patch
    // a ap b bp
    emit(a);
    emit(ap);
    emit(b);
    emit(bp);
    EndPrimitive();

}
