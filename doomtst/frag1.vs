#version 330 core
out vec4 FragColor;

uniform vec4 Color;
in vec3 ourcoord;
uniform sampler2DArray tex;

void main()
{
    FragColor =texture(tex,  vec3( ourcoord.x,ourcoord.y, ourcoord.z));
} 