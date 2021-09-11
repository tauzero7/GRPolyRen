#version 430

#include <shader/pattern.glsl>
#include <shader/objectcolor.glsl>

in vec3 vNormal;
in vec2 vTexCoords;

layout(location = 0) out vec4 fragColor;

void main() {
    vec4 color = objectcolor(vTexCoords, vNormal);
    fragColor = color;
}
