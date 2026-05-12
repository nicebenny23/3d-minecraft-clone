#version 330 core
layout (location= 0 ) in vec2 coord;


uniform vec2 scale;
uniform vec2 center;
uniform float aspectratio;
out vec2 frag_coord;



void main()
{
   //because texcoords are inverted
vec2 vec_pos= coord;
vec4 pos = vec4(coord*scale+2*  center,1, 1.0);
pos.y=pos.y*aspectratio;
gl_Position=pos;
    frag_coord = coord;
}