#version 330 core
out vec4 FragColor;


in vec3 fragcoord;
uniform sampler2DArray tex;
in vec3 Color;

float near = 0.23; 
float far  = 32; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


void main()
{
  float depth = -LinearizeDepth(gl_FragCoord.z) ; 
  depth=(exp(depth/8));
  vec4 col= vec4(depth,depth,depth,1);
  col =vec4(1,1,1,1);
    FragColor =vec4((Color+1)/16,1)*texture(tex,  vec3( fragcoord))*col;
  
} 