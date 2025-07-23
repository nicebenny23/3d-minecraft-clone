#version 330 core
layout (location = 0) in vec3 aPos;
layout (location= 1 ) in vec2 texcoord;
out vec2 fragcoord;

uniform float aspectratio;
uniform mat4 view;
uniform mat4 projection;

void main()
{
vec4 pos =  projection * view * vec4(aPos.x,aPos.y,aPos.z, 1.0);

pos.y=pos.y*aspectratio;

gl_Position=pos;
fragcoord = texcoord;
}