#version 330 core
out vec4 FragColor;


in vec2 ourcoord;
uniform sampler2D tex;

uniform vec3 col;

void main()
{
 
    FragColor = vec4(col,1)*texture(tex,ourcoord);
     
} 