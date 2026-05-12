#version 330 core
out vec4 FragColor;


in vec2 ourcoord;
uniform sampler2D tex;

uniform vec4 color;

void main()
{
 
    vec4 col = color*texture(tex,vec2(ourcoord.x,1-ourcoord.y));
     if(col.a<.1f){
     discard;
     }
     FragColor=col;
} 