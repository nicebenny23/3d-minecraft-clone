
#version 330 core
out vec4 FragColor;
in vec3 ourcoord;
uniform sampler2DArray tex;
uniform vec4 color;
void main()
{
    FragColor = texture(tex,ourcoord)*color;
} 