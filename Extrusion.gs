#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 14) out;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 light_position;
uniform float light_radius;

void main() {
    
    // Get triangle coordinates
    vec3 a = gl_in[0].gl_Position.xyz;
    vec3 b = gl_in[1].gl_Position.xyz;
    vec3 c = gl_in[2].gl_Position.xyz;

    // Compute center of mass
    vec3 d = (a + b + c) / 3.0;

    // Extrude vertices w.r.t light position
    vec3 ap = a + normalize(a - light_position) * (2.0 * light_radius);
    vec3 bp = b + normalize(b - light_position) * (2.0 * light_radius);
    vec3 cp = c + normalize(c - light_position) * (2.0 * light_radius);
    vec3 dp = d + normalize(d - light_position) * (3.0 * light_radius);
    
    // TODO is it possible to do this using only one patch?

    // Emit first patch
    // c b cp bp dp ap cp a c b
    gl_Position = projection * view * vec4(c, 1.0);
    EmitVertex();
    gl_Position = projection * view * vec4(b, 1.0);
    EmitVertex();
    gl_Position = projection * view * vec4(cp, 1.0);
    EmitVertex();
    gl_Position = projection * view * vec4(bp, 1.0);
    EmitVertex();
    gl_Position = projection * view * vec4(dp, 1.0);
    EmitVertex();
    gl_Position = projection * view * vec4(ap, 1.0);
    EmitVertex();
    gl_Position = projection * view * vec4(cp, 1.0);
    EmitVertex();
    gl_Position = projection * view * vec4(a, 1.0);
    EmitVertex();
    gl_Position = projection * view * vec4(c, 1.0);
    EmitVertex();
    gl_Position = projection * view * vec4(b, 1.0);
    EmitVertex();
    EndPrimitive();

    // Emit second patch
    // a ap b bp
    gl_Position = projection * view * vec4(a, 1.0);
    EmitVertex();
    gl_Position = projection * view * vec4(ap, 1.0);
    EmitVertex();
    gl_Position = projection * view * vec4(b, 1.0);
    EmitVertex();
    gl_Position = projection * view * vec4(bp, 1.0);
    EmitVertex();
    EndPrimitive();

}
