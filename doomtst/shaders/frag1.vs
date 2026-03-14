#version 330 core
out vec4 FragColor;


in vec3 fragcoord;

uniform sampler2DArray tex;
in float Color;
in vec3 rendered_pos; 
uniform int render_distance;


float world_depth(float depth){
float modified_depth=length(rendered_pos);
float dv=min(1,max(0,modified_depth/((render_distance)*16)));
return 1-pow(dv,1.7)*(1-1);
}

void main()
{
  float depth = world_depth(gl_FragCoord.z)*Color; 
  vec4 col= vec4(depth,depth,depth,1)*texture(tex,fragcoord);
    FragColor =col;
  
} 