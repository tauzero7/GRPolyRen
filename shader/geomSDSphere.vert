#version 330

uniform mat4 projMX;
uniform mat4 viewMX;
uniform mat4 modelMX;

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec3 in_normal;

smooth out vec4 vEyeSpacePosVertex;
smooth out vec3 vPosition;
smooth out vec3 vNormal;

void main() {
    vec4 mvert = modelMX * in_position;
      
    vEyeSpacePosVertex = viewMX * mvert;
    gl_Position = projMX * vEyeSpacePosVertex;
    vPosition = mvert.xyz / mvert.w;
    vNormal = in_normal;
}
