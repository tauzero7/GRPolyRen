#version 330

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec4 in_color;

uniform mat4 projMX;
uniform mat4 viewMX;
uniform mat4 modelMX;

out vec4 vColor;

void main() {
    vec4 verts = in_position;
    gl_Position = projMX * viewMX * modelMX * verts;
    vColor = in_color;
}