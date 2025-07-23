#version 330 core
out vec4 FragColor;


in vec2 fragcoord;
uniform sampler2D tex;


void main()
{
    FragColor =vec4(1,1,1,1)*texture(tex,fragcoord);
 
} 