#version 330 core
layout (location = 0) in vec3 aPos;
layout (location= 1 ) in vec3 texcoord;
layout (location= 2 ) in float col;
out float Color;
out vec3 fragcoord;
out vec3 rendered_pos;

uniform float aspectratio;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 camera_pos;
void main()
{
   

      vec4 pos =  projection * view * vec4(aPos.x,aPos.y,aPos.z, 1.0);

pos.y=pos.y*aspectratio;
rendered_pos=camera_pos-aPos;
gl_Position=pos;


    Color = col;
    fragcoord = texcoord;
}