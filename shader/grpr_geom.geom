#version 430

#include <shader/schwarzschild.glsl>

uniform mat4 projMX;
uniform mat4 viewMX;
uniform mat4 modelMX;
uniform vec3 obsCamPos;
uniform vec3 main_e2;

uniform float imageOrder;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vPosition[];
in vec3 vNormal[];
in vec2 vTexCoords[];

out vec3 gPosition;
out vec3 gNormal;
out vec2 gTexCoords;

void main() {
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;

    vec3 p0 = calcApparentPos(obsCamPos, v0.xyz, imageOrder, 0.8);
    vec3 p1 = calcApparentPos(obsCamPos, v1.xyz, imageOrder, 0.8);
    vec3 p2 = calcApparentPos(obsCamPos, v2.xyz, imageOrder, 0.8);

    gPosition = vPosition[0];
    gNormal = vNormal[0];
    gTexCoords = vTexCoords[0];
    gl_Position = projMX * viewMX * vec4(p0, 1);
    EmitVertex();

    gPosition = vPosition[1];
    gNormal = vNormal[1];
    gTexCoords = vTexCoords[1];
    gl_Position = projMX * viewMX * vec4(p1, 1);
    EmitVertex();

    gPosition = vPosition[2];
    gNormal = vNormal[2];
    gTexCoords = vTexCoords[2];
    gl_Position = projMX * viewMX * vec4(p2, 1);
    EmitVertex();

    EndPrimitive();
}
