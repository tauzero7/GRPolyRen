#version 430

const float PI = 3.1415926;

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texCoords;

uniform mat4 projMX;
uniform mat4 viewMX;
uniform mat4 modelMX;

out vec3 vNormal;
out vec2 vTexCoords;

void main() {
    vec4 vert = modelMX * in_position;

    vec3 observerCamPos = vec3(10.0, 0.0, 0.0);

    vec3 e1 = normalize(observerCamPos);
    vec3 e3 = cross(e1, vert.xyz);
    vec3 e2 = normalize(cross(e3, e1));

    float x = dot(vert.xyz, e1);
    float y = dot(vert.xyz, e2);
    float r = sqrt(x*x + y*y);
    float phi = atan(y, x);

    vec3 p = r * cos(phi) * e1 + r * sin(phi) * e2;
    vert = vec4(p, 1);

    gl_Position = projMX * viewMX * vert;

    vNormal = (modelMX * vec4(in_normal, 0)).xyz;
    vTexCoords = in_texCoords;
}