#version 330 core
layout (location= 0 ) in vec2 texcord;


uniform float scale;
uniform vec2 center;
uniform float aspectratio;
out vec2 texcoord;



void main()
{
   //because texcoords are inverted
vec2 vec_pos= (vec2(0.5,0.5)-texcord)*2;
vec4 pos = vec4(vec_pos*scale+center,1, 1.0);
pos.y=pos.y*aspectratio;
gl_Position=pos;
    texcoord = texcord;
}