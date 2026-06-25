#version 330 core
out vec4 FragColor;


in vec3 fragcoord;

uniform sampler2DArray tex;
in float light;
in vec3 rendered_pos; 
uniform int render_distance;


float world_depth(float depth){
float modified_depth=length(rendered_pos);
float dv=min(1,max(0,modified_depth/((render_distance)*16)));
return pow(dv,2.5f);
}

void main()
{
  float block_depth=min(1,length(rendered_pos)/4);
  float depth=world_depth(gl_FragCoord.z);
  float inv_depth=1-depth;
  float ambient=.16;
  if(block_depth>.99){

  }
    else{
    ambient+=pow(1-block_depth,1.3)*.1;
  }
  float color=min(1,ambient+light)*inv_depth;
  vec4 col= vec4(color,color,color,1)*texture(tex,fragcoord);
    FragColor=col;
  
} 