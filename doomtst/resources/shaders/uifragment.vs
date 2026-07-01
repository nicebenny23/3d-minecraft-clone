#version 330 core
out vec4 FragColor;


in vec2 frag_coord;
uniform vec4 color;
uniform sampler2D tex;



void main()
{
 
    vec2 coord=frag_coord;
    coord.y*=-1;

    FragColor =texture(tex,  coord/2+vec2(.5,.5))*color;
} 