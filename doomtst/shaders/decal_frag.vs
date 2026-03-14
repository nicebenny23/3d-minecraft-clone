#version 330 core
out vec4 FragColor;


in vec2 fragcoord;
uniform sampler2D tex;


void main()
{
    vec4 col=texture(tex,fragcoord);
    if(col.a < 0.01){
    discard;
    }
    FragColor =col;
 
} 