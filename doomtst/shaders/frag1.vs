#version 330 core
out vec4 FragColor;


in vec3 fragcoord;
uniform sampler2DArray tex;
in vec3 Color;


void main()
{
 
    FragColor =vec4((Color+3)/16,1)*texture(tex,  vec3( fragcoord));
} 