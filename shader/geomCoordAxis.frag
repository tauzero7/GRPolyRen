#version 330

uniform vec3 color;

layout(location = 0) out vec4 fragColor;

smooth in float dotProd;

void main() {
    fragColor = vec4(color,1);    
    fragColor = vec4(vec3(dotProd) * color,1);    
}

