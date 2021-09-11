#version 330

uniform mat4 invMVMX;
uniform vec4 m_color;
uniform float flatShading;

layout(location = 0) out vec4 fragColor;

smooth in vec4 vEyeSpacePosVertex;
smooth in vec3 vPosition;
smooth in vec3 vNormal;

void main() {
    vec4 color = vec4(vNormal, 1.0);
    float fc = abs(vEyeSpacePosVertex.z / vEyeSpacePosVertex.w);
            
    vec3 camera  = invMVMX[3].xyz;    
    vec3 ray_dir = normalize(camera - vPosition);    
    float val = dot(ray_dir, vNormal);
    val = mix(pow(val, 0.7), 1.0, flatShading);
        
    color = m_color * vec4(vec3(val), 1);
    fragColor = color;    
}
