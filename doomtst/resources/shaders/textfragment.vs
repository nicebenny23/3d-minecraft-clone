
#version 330 core
out vec4 FragColor;
in vec3 outcoord;
uniform sampler2DArray tex;
uniform vec4 color;
void main()
{
    FragColor = texture(tex,outcoord)*color;
} 