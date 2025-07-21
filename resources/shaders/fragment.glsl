// fragment.glsl
#version 330 core

in vec2 vTexCoord;       // ← new
out vec4 FragColor;

uniform sampler2D uTexture; // ← new

void main()
{
    // sample the texture instead of a flat color:
    FragColor = texture(uTexture, vTexCoord);
}
