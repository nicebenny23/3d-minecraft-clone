#version 330 core
layout (location = 0) in vec2 aPos;
layout (location= 1 ) in vec2 texcord;


  

out vec2 texcoord;



void main()
{
   

      
gl_Position =  vec4(aPos.x,aPos.y*1.777,1, 1.0);


    
    texcoord = texcord;
}