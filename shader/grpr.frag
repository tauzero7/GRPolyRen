#version 430

#include <shader/pattern.glsl>
#include <shader/objectcolor.glsl>
#include <shader/schwarzschild.glsl>

struct LightSource {
    float is_active;
    vec3 position;
    float factor;
};

uniform LightSource light1;

uniform float imageOrder;

in vec3 gPosition;
in vec3 gNormal;
in vec2 gTexCoords;

layout(location = 0) out vec4 fragColor;

float getLight(vec3 pos) {
    vec3 lightDir1_o0, lightDir1_o1;
    float dist_o0, dist_o1;
    calcApparentDirAndDist(light1.position, gPosition, 0.0, lightDir1_o0, dist_o0);
    calcApparentDirAndDist(light1.position, gPosition, 1.0, lightDir1_o1, dist_o1);
    float val1_o0 = max(0, dot(gNormal, lightDir1_o0));
    float val1_o1 = max(0, dot(gNormal, lightDir1_o1));

    float val = val1_o0 / (dist_o0 * dist_o0) + val1_o1 / (dist_o1 * dist_o1);
    val *= light1.factor * 3e3;

    return mix(1.0, val, light1.is_active);
}

void main() {
    vec4 color = objectcolor(gTexCoords, gNormal);
    color.rgb *= getLight(gPosition);      
    fragColor = color;
}
