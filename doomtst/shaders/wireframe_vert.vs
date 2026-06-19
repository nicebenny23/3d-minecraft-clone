#version 330 core
layout (location = 0) in vec3 aPos;


  

out vec2 ourcoord;
uniform float aspectratio;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 pos =  projection * view*model* vec4(aPos.x,aPos.y,aPos.z, 1.0);
    pos.z-=.0004f;
    gl_Position=pos;
    
}