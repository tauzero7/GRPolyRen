#version 430

#include <shader/pattern.glsl>
#include <shader/objectcolor.glsl>
#include <shader/schwarzschild.glsl>

in vec3 gPosition;
in vec3 gNormal;
in vec2 gTexCoords;

layout(location = 0) out vec4 fragColor;

void main() {
    vec4 color = objectcolor(gTexCoords, gNormal);
    fragColor = color;
}
