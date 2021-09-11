#version 430

uniform float distRelation;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 tPosition[];
in vec3 tNormal[];
in vec2 tTexCoords[];
in vec3 tPosScreenSpace[];

out vec3 gPosition;
out vec3 gNormal;
out vec2 gTexCoords;

float getCircumDist(vec3 p0, vec3 p1, vec3 p2) {
    return length(p0 - p1) + length(p1 - p2) + length(p2 - p0);
}

void main() {
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;

    float distPrev = getCircumDist(tPosScreenSpace[0], tPosScreenSpace[1], tPosScreenSpace[2]);
    float distNew = getCircumDist(v0.xyz, v1.xyz, v2.xyz);
    float factor = 1.0;
    if (distNew / distPrev > distRelation) {
        factor = 0.0;
    }

    gPosition = tPosition[0];
    gNormal = tNormal[0];
    gTexCoords = tTexCoords[0];
    gl_Position = factor * v0;
    EmitVertex();

    gPosition = tPosition[1];
    gNormal = tNormal[1];
    gTexCoords = tTexCoords[1];
    gl_Position = factor * v1;
    EmitVertex();

    gPosition = tPosition[2];
    gNormal = tNormal[2];
    gTexCoords = tTexCoords[2];
    gl_Position = factor * v2;
    EmitVertex();
 
    EndPrimitive();
}
