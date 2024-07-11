#version 330 core
out vec4 FragColor;


in vec3 ourcoord;
uniform sampler2DArray tex;
in vec3 Color;
float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


void main()
{
 
    FragColor =vec4(Color/16+.1,1)*texture(tex,  vec3( ourcoord.x,ourcoord.y, ourcoord.z));
} 