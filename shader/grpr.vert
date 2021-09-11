#version 430

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texCoords;

uniform mat4 modelMX;

out vec3 vNormal;
out vec2 vTexCoords;


void main() {
    gl_Position = modelMX * in_position;

    vNormal = (modelMX * vec4(in_normal, 0)).xyz;
    //vNormal = in_normal;
    vTexCoords = in_texCoords;
}
