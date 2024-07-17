#version 330 core
layout (location = 0) in vec3 aPos;
layout (location= 1 ) in vec3 texcoord;
layout (location= 2 ) in float col;
out vec3 Color;

  

out vec3 fragcoord;


uniform mat4 view;
uniform mat4 projection;
uniform vec3 aColor;
void main()
{
   

      vec4 pos =  projection * view * vec4(aPos.x,aPos.y,aPos.z, 1.0);
pos.y=pos.y*1.77;

gl_Position=pos;


    Color = vec3(col,col,col);
    fragcoord = texcoord;
}