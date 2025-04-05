#version 330 core
layout (location = 0) in vec2 aPos;
layout (location= 1 ) in vec2 texcord;


  //width/height
  uniform float aspectratio;
out vec2 texcoord;



void main()
{
   

  vec4 pos=  vec4(aPos.x,aPos.y,1, 1.0);

pos.y=pos.y*aspectratio;

    
gl_Position=pos;
    texcoord = texcord;
}