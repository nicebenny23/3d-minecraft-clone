
#version 330 core
out vec4 FragColor;
in vec3 ourcoord;
uniform sampler2DArray tex;
void main()
{
 
    FragColor = texture(tex,ourcoord);
} 