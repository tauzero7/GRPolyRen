#version 330

in vec4 vColor;

layout(location = 0) out vec4 fragColor;

void main() {
    vec4 color = vec4(1);
    color = vColor;
    fragColor = color;
}