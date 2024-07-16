
#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 texcoord;

out vec2 TexCoords;

uniform mat4 projection;
uniform vec3 offset;

void main()
{
    float scale = 10.0f;
    TexCoords = texcoord;

    gl_Position = projection * vec4(vertex*scale, 1.0);
}