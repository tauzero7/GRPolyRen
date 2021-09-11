#version 330

layout(location = 0) in vec3  in_position;
layout(location = 1) in vec3  in_normals;

uniform mat4  modelMX;
uniform mat4  viewMX;
uniform mat4  projMX;
uniform mat4  invMV;

uniform float scale;
uniform float offset;
uniform float length;

smooth out float dotProd;

void main() {
    vec4 vert = vec4(in_position.xy * scale, in_position.z * length + offset, 1.0);
    vert  = modelMX * vert;
    gl_Position = projMX * viewMX * vert;    
    
    vec3 camPos = invMV[3].xyz;
    dotProd = max(0, dot(normalize(camPos), in_normals));
}

