// vertex.glsl
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;    // ← new

out vec2 vTexCoord;                         // ← new

uniform vec2 uOffset;
uniform float uScale;
uniform float uRotation; // in radians

void main()
{
    // build a simple 2D rotation matrix:
    float c = cos(uRotation);
    float s = sin(uRotation);
    mat2 rot = mat2(c, -s,
                    s,  c);

    vec2 pos2D = (rot * (aPos.xy * uScale)) + uOffset;

    gl_Position = vec4(pos2D, aPos.z, 1.0);

    vTexCoord = aTexCoord;                  // ← pass the UV through
}
