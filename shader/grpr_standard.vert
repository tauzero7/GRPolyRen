#version 430

#include <shader/schwarzschild.glsl>

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texCoords;

uniform mat4 projMX;
uniform mat4 viewMX;
uniform mat4 modelMX;
uniform vec3 obsCamPos;
uniform vec3 mainNormal;

uniform float imageOrder;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoords;


void main() {
    vec4 vert = modelMX * in_position;

    vec3 apparentPos = calcApparentPos(obsCamPos, vert.xyz, imageOrder, 0.8);
    vert = vec4(apparentPos, 1);

    gl_Position = projMX * viewMX * vert;

    vPosition = vert.xyz;
    vNormal = (modelMX * vec4(in_normal, 0)).xyz;
    vTexCoords = in_texCoords;
}
