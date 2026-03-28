#version 330 core
out vec4 FragColor;


in vec2 ourcoord;
uniform sampler2D tex;

uniform vec4 color;

void main()
{
 
    FragColor = color*texture(tex,ourcoord);
     
} 