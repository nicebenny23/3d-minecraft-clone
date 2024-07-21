#version 330 core
layout (location = 0) in vec3 aPos;
layout (location= 1 ) in vec2 texcord;


  

out vec2 ourcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   

      vec4 pos =  projection * view*model* vec4(aPos.x,aPos.y,aPos.z, 1.0);
      pos.y=pos.y*1.77;
gl_Position=pos;



    ourcoord = texcord;
}