#version 330 core
layout (location = 0) in vec3 aPos;
layout (location= 1 ) in vec2 texcord;

out vec3 Color;

  

out vec2 ourcoord;


uniform mat4 view;
uniform mat4 projection;
uniform vec3 aColor;
void main()
{
   

      
gl_Position =  projection * view * vec4(aPos, 1.0);


    Color = aColor;
    ourcoord = texcord;
}